#include <dk_buttons_and_leds.h>
#include <zephyr/smf.h>
#include <zephyr/net/openthread.h>
#include <openthread/thread.h>
#include <openthread/coap.h>
#include <zephyr/drivers/gpio.h>

#include "sensor_functionality.h"

#define SLEEP_TIME_ONE 1
#define SLEEP_TIME_FIVE 5
#define SLEEP_TIME_TEN 10
#define SLEEP_TIME_THIRTY 30
#define SLEEP_TIME_SIXTY 60
#define SLEEP_TIME_THREE_MIN 180
#define NO_SLEEP_TIME 0

#define NO_ERROR 0

#define NO_BLINKING_LED 5

LOG_MODULE_REGISTER(main, LOG_LEVEL_INF);

/**
 * Boolean for printing read sensor values to the console
 * Change to true to enable
*/
static bool print_values_to_console = false;

/**
 * Modes for the Sensor Station
 * 
 * Measure: 		Start/stop periodic measurement
 * Config_period: 	Change the measurement period of the Sensor Station
 * Calibrate: 		Start/stop the selected calibration mode (only Co2 available)
 * 
*/
static enum sensor_station_mode { 
	MEASURE, 
	CONFIG_PERIOD, 
	CALIBRATE
} station_mode;

/* Variables used for managing modes, LEDs, Buttons and States of the SMF */
static bool mode_switch;

static bool btn2_pressed;
static bool btn3_pressed;
static bool btn4_pressed;

static bool run_calibration;
static bool run_per_measurement;
static bool run_frc;

static bool new_led_state;
static uint8_t blinking_led;

static struct k_work blinking_led_3sec_work;
static struct k_timer blinking_led_timer;

static bool show_idle_state;

/* Functions for Handling LEDs */

/**
 * set_blinking_led() - Sets the selected blinking_led to blinking ON/OFF.
*/
static void set_blinking_led() {

	k_timer_stop(&blinking_led_timer);
	switch(station_mode) {
		case CONFIG_PERIOD:
			switch (measure_period) {
				case SIXTY:
					blinking_led = DK_LED4;
					break;
				case THIRTY:
					blinking_led = DK_LED3;
					break;
				case FIVE:
				default:
					blinking_led = DK_LED1;
			}
			break;

		case CALIBRATE:
			switch (calib_mode) {
				case VOC:
					blinking_led = DK_LED2;
					break;
				case TEMP:
					blinking_led = DK_LED4;
					break;
				case CO2:
				default:
					blinking_led = DK_LED1;
			}
			break;
		case MEASURE:
		default:
			blinking_led = NO_BLINKING_LED; 		
	}
	if(blinking_led != NO_BLINKING_LED) {
		k_timer_start(&blinking_led_timer, K_MSEC(500), K_MSEC(500));
	}
}

/**
 * set_select_led() - Sets the selected blinking_led to blinking ON.
*/
static void set_select_led() {

	k_timer_stop(&blinking_led_timer);
	switch(station_mode) {
		case CONFIG_PERIOD:
			switch (measure_period) {
				case SIXTY:
					blinking_led = DK_LED4;
					break;
				case THIRTY:
					blinking_led = DK_LED3;
					break;
				case FIVE:
				default:
					blinking_led = DK_LED1;
			}
			break;

		case CALIBRATE:
			switch (calib_mode) {
				case VOC:
					blinking_led = DK_LED2;
					break;
				case TEMP:
					blinking_led = DK_LED4;
					break;
				case CO2:
				default:
					blinking_led = DK_LED1;
			}
			break;
		case MEASURE:
		default:
			blinking_led = DK_LED2; 		
	}
	
	dk_set_led_on(blinking_led);
}

/**
 * stop_blinking_led() - Sets the selected blinking_led to blinking OFF.
*/
static void stop_blinking_led() {

	k_timer_stop(&blinking_led_timer);
	dk_set_led_off(blinking_led);
}

static void activate_blinking_3_sec(struct k_work *item)
{
	ARG_UNUSED(item);

	set_blinking_led();
	k_sleep(K_SECONDS(SLEEP_TIME_FIVE));
	stop_blinking_led();
}

/* Expiry function of blinking_led_timer */
static void on_blinking_led_timer_expiry(struct k_timer *timer_id)
{
	static uint8_t val = 1;

	ARG_UNUSED(timer_id);

	dk_set_led(blinking_led, val);
	val = !val;
}

/* Stop function of blinking_led_timer */
static void on_blinking_led_timer_stop(struct k_timer *timer_id)
{
	ARG_UNUSED(timer_id);

	dk_set_led_off(blinking_led);
}


/* Button Handler */
static void on_button_changed(uint32_t button_state, uint32_t has_changed)
{
	uint32_t buttons = button_state & has_changed;
	
	if (buttons & DK_BTN1_MSK) {
		mode_switch = true;
	}
	if (buttons & DK_BTN2_MSK) {
		btn2_pressed = true;
	}
	if (buttons & DK_BTN3_MSK) {
		btn3_pressed = true;
	}
	if (buttons & DK_BTN4_MSK) {
		btn4_pressed = true;
	}

}

/* Forward declaration of helper function*/
static const char* station_mode_to_string(enum sensor_station_mode s_mode);
static bool sleep_interruptable(uint8_t sec, bool is_for_calibration);


/* CoAP functionality */
// VM-IP-ADDRESS
const char *serverIpAddr = "fd56:b1f:f132:2:0:0:10.80.2.239";

void coap_init(void);
void coap_send_data_response_cb(void *p_context, otMessage *p_message, const otMessageInfo *p_message_info, otError result);
void coap_send_data_request(char *message);


/* State Machine Framework */

int32_t smf_sleep_sec = NO_SLEEP_TIME;

/* Forward declaration of state table */
static const struct smf_state states[];

/* States */
enum state { 
	INIT, 
	IDLE, 
	START_MEASUREMENT, 
	STOP_MEASUREMENT, 
	READ_MEASUREMENT, 
	SEND_DATA, 
	START_CALIB_MEASUREMENT, 
	CALIB_READY, 
	FRC
};

struct s_object {
	/* This must be first */
	struct smf_ctx ctx;
	/* Error Value */
	int16_t error;
} s_obj;

/* State INIT */
static void init_run(void *o){
	printk("State: INIT\n");

	blinking_led = DK_LED1;
	k_timer_start(&blinking_led_timer, K_MSEC(500), K_MSEC(500));
	
	struct s_object *s = (struct s_object *)o;
	s->error = NO_ERROR;


	/* Init Settings for Sensor Station */
	station_mode = MEASURE;
	mode_switch = false;
	btn2_pressed = false;
	btn3_pressed = false;
	btn4_pressed = false;

	measure_period = FIVE;
	calib_mode = CO2;

	run_per_measurement = false;
	run_calibration = false;
	run_frc = false;

	/* Init Sensor Measuring */
	sensirion_i2c_hal_get();
	clean_up_sensor_states(&(s->error));
	if(s->error) {
		printk("Error executing clean_up_sensor_states(): %i\n", s->error);
		smf_set_state(SMF_CTX(&s_obj), &states[IDLE]);
		return;
	}

	/* Init CoAP */
	coap_init();

	k_timer_stop(&blinking_led_timer);
	smf_sleep_sec = SLEEP_TIME_FIVE;
	new_led_state = true;
	show_idle_state = true;
	smf_set_state(SMF_CTX(&s_obj), &states[IDLE]);
}


/* State IDLE */
static void idle_entry(void *o) {
	if(show_idle_state) {
		printk("State: IDLE\tMode: %s\n", station_mode_to_string(station_mode));
		show_idle_state = false;
		}
}

static void idle_run(void *o){

	struct s_object *s = (struct s_object *)o;
	s->error = NO_ERROR;
	
	switch(station_mode) {

		/* Mode CONFIG_PERIOD */
		case CONFIG_PERIOD:
			dk_set_led_on(DK_LED2);
			if(btn2_pressed) {
				measure_period = FIVE;
				printk("measure_period Change: FIVE\n");
				new_led_state = true;
				btn2_pressed = false;
			}
			if(btn3_pressed) {
				measure_period = THIRTY;
				printk("measure_period Change: THIRTY\n");
				new_led_state = true;
				btn3_pressed = false;
			}
			if(btn4_pressed) {
				measure_period = SIXTY;
				printk("measure_period Change: SIXTY\n");
				new_led_state = true;
				btn4_pressed = false;
			}
			if(new_led_state) {
				dk_set_leds(DK_NO_LEDS_MSK);
				set_blinking_led();
				new_led_state = false;
			}
			break;

		/* Mode CALIBRATE */
		case CALIBRATE:
			dk_set_led_on(DK_LED3);
			if(btn2_pressed) {
				if(calib_mode == CO2 && run_calibration) {
					smf_set_state(SMF_CTX(&s_obj), &states[START_CALIB_MEASUREMENT]);
					smf_sleep_sec = SLEEP_TIME_ONE;
					btn2_pressed = false;
					new_led_state = true;
					return;
				}
				else if(calib_mode == CO2) {
					printk("User Select CO2 Calibration\n");
					run_calibration = true;
					set_select_led();
				}
				else {
					printk("calib_mode Change: CO2\n");
					calib_mode = CO2;
					run_calibration = false;
					new_led_state = true;
				}
				btn2_pressed = false;
			}
			if(btn3_pressed) {
				if(calib_mode == VOC && run_calibration) {
					smf_set_state(SMF_CTX(&s_obj), &states[START_CALIB_MEASUREMENT]);
					smf_sleep_sec = SLEEP_TIME_ONE;
					btn3_pressed = false;
					new_led_state = true;
					return;
				}
				else if(calib_mode == VOC) {
					printk("User Select VOC Calibration\n");
					run_calibration = true;
					set_select_led();
				}
				else {
					printk("calib_mode Change: VOC\n");
					calib_mode = VOC;
					run_calibration = false;
					new_led_state = true;
				}
				btn3_pressed = false;
			}
			if(btn4_pressed) {
				if(calib_mode == TEMP && run_calibration) {
					smf_set_state(SMF_CTX(&s_obj), &states[START_CALIB_MEASUREMENT]);
					smf_sleep_sec = SLEEP_TIME_ONE;
					btn4_pressed = false;
					new_led_state = true;
					return;
				}
				else if(calib_mode == TEMP) {
					printk("User Select TEMP Calibration\n");
					run_calibration = true;
					set_select_led();
				
				}
				else {
					printk("calib_mode Change: TEMP\n");
					calib_mode = TEMP;
					run_calibration = false;
					new_led_state = true;
				}
				btn4_pressed = false;
			}
			if(new_led_state) {
				dk_set_leds(DK_NO_LEDS_MSK);
				set_blinking_led();
				new_led_state = false;
			}
			break;


		/* Mode MEASURE */
		case MEASURE:
		default:
			dk_set_led_on(DK_LED1); 		
			if(btn2_pressed) {
				if(run_per_measurement) {
					smf_set_state(SMF_CTX(&s_obj), &states[START_MEASUREMENT]);
					smf_sleep_sec = SLEEP_TIME_ONE;
					btn2_pressed = false;
					return;
				}
				else {
					printk("User Select Start Measurement\n");
					run_per_measurement = true;
					set_select_led(blinking_led);
				}
				btn2_pressed = false;
			}
			if(btn3_pressed) {
				// do nothing
				run_per_measurement = false;
				btn3_pressed = false;
				new_led_state = true;
			}
			if(btn4_pressed) {
				// do nothing
				run_per_measurement = false;
				btn4_pressed = false;
				new_led_state = true;
			}
			if(new_led_state) {
				dk_set_leds(DK_NO_LEDS_MSK);
				set_blinking_led();
				new_led_state = false;
			}
			break;
	}
	
	smf_sleep_sec = NO_SLEEP_TIME;
	smf_set_state(SMF_CTX(&s_obj), &states[IDLE]);
}

/* State START_MEASUREMENT */
static void start_measurement_run(void *o){
	printk("State: START_MEASUREMENT\tmeasure_period: %s\n", measure_period_to_string(measure_period));

	struct s_object *s = (struct s_object *)o;
	s->error = NO_ERROR;

	dk_set_leds(DK_NO_LEDS_MSK);
	stop_blinking_led();
	blinking_led = DK_LED1;
	
	/* Start periodic measurement */
	start_periodic_measurement(&(s->error), true);
	if(s->error) {
		printk("Error executing start_periodic_measurement()\n: %i", s->error);
		return;
	}

	if(sleep_interruptable(SLEEP_TIME_TEN, false)) {
		smf_set_state(SMF_CTX(&s_obj), &states[STOP_MEASUREMENT]);
		smf_sleep_sec = NO_SLEEP_TIME;
		return;
	}

	smf_sleep_sec = NO_SLEEP_TIME;
	smf_set_state(SMF_CTX(&s_obj), &states[READ_MEASUREMENT]);
}

/* State STOP_MEASUREMENT */
static void stop_measurement_run(void *o){
	printk("State: STOP_MEASUREMENT\n");
	
	struct s_object *s = (struct s_object *)o;
	s->error = NO_ERROR;

	stop_blinking_led();

	/* Stop periodic measurement */
	stop_periodic_measurement(&(s->error));
	if(s->error) {
		printk("Error executing stop_periodic_measurement(): %i\n", s->error);
		return;
	}

	dk_set_leds(DK_ALL_LEDS_MSK);
	
	run_calibration = false;
	run_per_measurement = false;
	run_frc = false;

	new_led_state = true;
	smf_sleep_sec = NO_SLEEP_TIME;
	
	show_idle_state = true;
	smf_set_state(SMF_CTX(&s_obj), &states[IDLE]);
}

/* State READ_MEASUREMENT */
static void read_measurement_run(void *o){
	printk("State: READ_MEASUREMENT\n");

	struct s_object *s = (struct s_object *)o;
	s->error = NO_ERROR;

	stop_blinking_led();

	/* for measure_period = SIXTY, a single shot is performed */
	if(measure_period == SIXTY) {
		perform_single_measurement_scd41(&(s->error));
		if(s->error) {
			printk("Error executing perform_single_measurement_scd41(): %i\n", s->error);
			//return;
		}
	}

	/* Read the Measurement */
	read_measurement(&(s->error));
	if(s->error) {
		printk("Error executing read_measurement(): %i\n", s->error);
		//return;
	}

	/* Invoke console print if print_values_to_console is enabled */
	if(print_values_to_console) {
		print_measurement_to_console();
	}

	smf_sleep_sec = NO_SLEEP_TIME;
	smf_set_state(SMF_CTX(&s_obj), &states[SEND_DATA]);
}

/* State SEND_DATA */
static void send_data_run(void *o){
	printk("State: SEND_DATA\n");

	struct s_object *s = (struct s_object *)o;
	s->error = NO_ERROR;
	
	// Use shared data for CoAP communication
	char* my_sensor_data;
	my_sensor_data = create_coap_message();
	coap_send_data_request(my_sensor_data);
	free(my_sensor_data);

	/** 
	 * Station sleeps for measure_period for another read/send-cycle to start.
	 * If the stop button (Button 4) or the Button for changing the
	 * Sensor Station mode (Button 1) is pressed while sleeping, stop measuring and
	 * return to idle.
	*/
	switch(measure_period) {
		case FIVE: 
			if(sleep_interruptable(SLEEP_TIME_FIVE, false)) {
				smf_set_state(SMF_CTX(&s_obj), &states[STOP_MEASUREMENT]);
				smf_sleep_sec = NO_SLEEP_TIME;
				return;
			}
			break;
		case THIRTY:
			if(sleep_interruptable(SLEEP_TIME_THIRTY, false)) {
				smf_set_state(SMF_CTX(&s_obj), &states[STOP_MEASUREMENT]);
				smf_sleep_sec = NO_SLEEP_TIME;
				return;
			}
			break;
		case SIXTY:
			if(sleep_interruptable(SLEEP_TIME_SIXTY, false)) {
				smf_set_state(SMF_CTX(&s_obj), &states[STOP_MEASUREMENT]);
				smf_sleep_sec = NO_SLEEP_TIME;
				return;
			}
			break;
	}
	smf_sleep_sec = NO_SLEEP_TIME;
	smf_set_state(SMF_CTX(&s_obj), &states[READ_MEASUREMENT]);
}

/* State START_CALIB_MEASUREMENT */
static void start_calib_measurement_run(void *o){
	printk("State: START_CALIB_MEASUREMEN, calib_mode: %s, measure_period: %s\n", calib_mode_to_string(calib_mode), measure_period_to_string(measure_period));
	struct s_object *s = (struct s_object *)o;
	s->error = NO_ERROR;

	set_select_led();

	switch(calib_mode) {
		case CO2:
			/* measure CO2 for 3 Minutes, interruptable with Button 1 (change mode) or 4 (stop) */
			start_periodic_measurement(&(s->error), false);
			if(s->error) {
				printk("Error executing start_periodic_measurement(): %i\n", s->error);
				return;
			}

			if(sleep_interruptable(SLEEP_TIME_THREE_MIN, true)) {
				smf_set_state(SMF_CTX(&s_obj), &states[STOP_MEASUREMENT]);
				smf_sleep_sec = NO_SLEEP_TIME;
				return;
			}

			stop_periodic_measurement(&(s->error));
			if(s->error) {
				printk("Error executing start_periodic_measurement(): %i\n", s->error);
				return;
			}
			stop_blinking_led();
			smf_sleep_sec = SLEEP_TIME_ONE;
			break;
		case VOC:
			/* Not implemented, thus disabled */
			break;
		case TEMP:
			/* Not implemented, thus disabled */
			break;
	}
	
	show_idle_state = true;
	smf_set_state(SMF_CTX(&s_obj), &states[CALIB_READY]);
}

/* State CALIB_READY */
static void calib_ready_run(void *o){
	if(show_idle_state){
		("State: CALIB_READY\n");
		show_idle_state = false;
	}

	struct s_object *s = (struct s_object *)o;
	s->error = NO_ERROR;
	set_blinking_led();
	switch(calib_mode) {
		case CO2:
			if(btn2_pressed) {
				if(run_frc) {
					smf_set_state(SMF_CTX(&s_obj), &states[FRC]);
					smf_sleep_sec = NO_SLEEP_TIME;
					btn2_pressed = false;
					return;
				}
				run_frc = true;
				set_select_led();
				break;
			}
			break;
		case VOC:
			/* Not implemented, thus disabled */
			break;
		case TEMP:
		default:
			/* Not implemented, thus disabled */
	}
	
	smf_sleep_sec = NO_SLEEP_TIME;
	smf_set_state(SMF_CTX(&s_obj), &states[CALIB_READY]);
}

/* State FRC */
static void frc_run(void *o){
	printk("State: FRC\n");
	
	struct s_object *s = (struct s_object *)o;
	s->error = NO_ERROR;
	stop_blinking_led();

	switch(calib_mode) {
		case CO2:
			forced_co2_recalibration(&(s->error));
			break;
		case VOC:
			/* Not implemented, thus disabled */
			break;
		case TEMP:
		default:
			/* Not implemented, thus disabled */
	}
	
	smf_sleep_sec = SLEEP_TIME_ONE;
	run_frc = false;
	new_led_state = true;
	
	show_idle_state = true;
	smf_set_state(SMF_CTX(&s_obj), &states[IDLE]);
}

/* Populate SMF state table */
static const struct smf_state states[] = {
	[INIT] = SMF_CREATE_STATE(NULL, init_run, NULL),
	[IDLE] = SMF_CREATE_STATE(idle_entry, idle_run, NULL),
	[START_MEASUREMENT] = SMF_CREATE_STATE(NULL, start_measurement_run, NULL),
	[STOP_MEASUREMENT] = SMF_CREATE_STATE(NULL, stop_measurement_run, NULL),
	[READ_MEASUREMENT] = SMF_CREATE_STATE(NULL, read_measurement_run, NULL),
	[SEND_DATA] = SMF_CREATE_STATE(NULL, send_data_run, NULL),
	[START_CALIB_MEASUREMENT] = SMF_CREATE_STATE(NULL, start_calib_measurement_run, NULL),
	[CALIB_READY] = SMF_CREATE_STATE(NULL, calib_ready_run, NULL),
	[FRC] = SMF_CREATE_STATE(NULL, frc_run, NULL)
};

/* main function */
int main(void)
{
	int32_t ret;

	/* Initialize timers on work queue for LEDs */
	k_timer_init(&blinking_led_timer, on_blinking_led_timer_expiry, on_blinking_led_timer_stop);
	k_work_init(&blinking_led_3sec_work, activate_blinking_3_sec);

	/* Initialize LEDs and Buttons */
	ret = dk_leds_init();
	if (ret) {
		printk("Could not initialize leds, err code: %d\n", ret);
		goto end;
	}

	ret = dk_buttons_init(on_button_changed);
	if (ret) {
		printk("Cannot init buttons (error: %d)\n", ret);
		goto end;
	}

	/* Set init SMF state */
	smf_set_initial(SMF_CTX(&s_obj), &states[INIT]);
	

	while (1) {
		/* run selected SMF state */
		ret = smf_run_state(SMF_CTX(&s_obj));
		if(ret) {
			printk("SMF Run State Error: %d\n", ret);
			smf_set_initial(SMF_CTX(&s_obj), &states[INIT]);
			smf_sleep_sec = SLEEP_TIME_ONE;
			continue;
		}

		/* End Application if an error occured during a state*/
		if(s_obj.error) {
			printk("Error Inside Run State: %d\n", ret);
			smf_set_state(SMF_CTX(&s_obj), &states[STOP_MEASUREMENT]);
			ret = smf_run_state(SMF_CTX(&s_obj));
			if(ret) {
				printk("SMF Run State Error: %d\n", ret);
			}
			goto end;
		}
		
		/* Stop, if the stop button (Button 4) is pressed while Sensor Station is running not in idle mode*/
		if(btn4_pressed && (run_calibration || run_per_measurement|| run_frc)) {
			smf_set_state(SMF_CTX(&s_obj), &states[STOP_MEASUREMENT]);
			ret = smf_run_state(SMF_CTX(&s_obj));
			smf_sleep_sec = NO_SLEEP_TIME;
			btn4_pressed = false;
		}

		/* Change Mode if Button 1 is pressed */
		if(mode_switch) {
			switch(station_mode) {
				case CONFIG_PERIOD: 
					station_mode = CALIBRATE;
					smf_sleep_sec = NO_SLEEP_TIME;
					smf_set_state(SMF_CTX(&s_obj), &states[IDLE]);
					break;

				case CALIBRATE: 	
					station_mode = MEASURE;
					smf_sleep_sec = NO_SLEEP_TIME;
					(run_calibration || run_frc) ? smf_set_state(SMF_CTX(&s_obj), &states[STOP_MEASUREMENT])
									: smf_set_state(SMF_CTX(&s_obj), &states[IDLE]);
					break;

				case MEASURE:
				default:	 		
					station_mode = CONFIG_PERIOD;
					smf_sleep_sec = NO_SLEEP_TIME;
					run_per_measurement ? smf_set_state(SMF_CTX(&s_obj), &states[STOP_MEASUREMENT])
										: smf_set_state(SMF_CTX(&s_obj), &states[IDLE]);
					new_led_state = true;
					break;
			};
			mode_switch = false;
			new_led_state = true;
			show_idle_state = true;
		}

		k_sleep(K_SECONDS(smf_sleep_sec));
	}
end:
	return 0;
}

/**
 * sleep_interruptable() - Helper method for interruptable sleep of a  SMF state
 * 
 * This method lets a SMF state sleeping, while checking every second, if
 * Button 1 or 4 have been pressed. If so, it stops the waiting time.
 * 
 * @param sec seconds for the SMF state to sleep
 * @param is_for_calibration when set true and the measure_period is SIXTY, the function
 * performs a single shot measurement every 60 seconds.
 * @returns false, if no interrupt happened, true otherwise.
*/
static bool sleep_interruptable(uint8_t sec, bool is_for_calibration) {

	for(uint8_t i = 0; i < sec; i++) {
		k_sleep(K_SECONDS(1));
		if(is_for_calibration && measure_period == SIXTY && i % 60 == 0) {
			/* for SIXTY second period, a single shot is performed */
			perform_single_measurement_scd41(&s_obj.error);
			if(s_obj.error) {
				printk("Error executing perform_single_measurement_scd41(): %i\n", s_obj.error);
				//return;
			}
		}
	
		if(btn4_pressed || mode_switch) {
			btn4_pressed = false;
			mode_switch = false;
			return true;
		}
	}
	return false;
}

/**
 * station_mode_to_string() - Helper function to change the enum sensor_station_mode to a string.
 * 
 * @param s_mode the current mode of the Sensor Station
 * @returns the String representation of the enum
*/
static const char* station_mode_to_string(enum sensor_station_mode s_mode) {
    switch (s_mode) {
        case MEASURE:
            return "MEASURE";
        case CONFIG_PERIOD:
            return "CONFIG_PERIOD";
        case CALIBRATE:
            return "CALIBRATE";
        default:
            return "Unknown";
    }
}

/* CoAP functions */

/* Initializes the CoAP functionality */
void coap_init(void)
{
	otInstance *p_instance = openthread_get_default_instance();
	otError error = otCoapStart(p_instance, OT_DEFAULT_COAP_PORT);
	if (error != OT_ERROR_NONE)
	{
		printk("Failed to start Coap: %d\n", error);
	}
}

/* Callback function for CoAP Server response handling */
void coap_send_data_response_cb(void *p_context, otMessage *p_message, const otMessageInfo *p_message_info, otError result)
{
	if (result == OT_ERROR_NONE)
	{
		printk("CoAP Response: Delivery confirmed.\n");
	}
	else
	{
		printk("CoAP Response: Delivery not confirmed: %d\n", result);
	}
}

/* Sends a message to the CoAP Server */
void coap_send_data_request(char *message)
{
	otError error = OT_ERROR_NONE;
	otMessage *myMessage;
	otMessageInfo myMessageInfo;
	otInstance *myInstance = openthread_get_default_instance();
	const otMeshLocalPrefix *ml_prefix = otThreadGetMeshLocalPrefix(myInstance);
	uint8_t serverInterfaceID[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01};

	const char *myTemperatureJson = message;
	do
	{
		myMessage = otCoapNewMessage(myInstance, NULL);
		if (myMessage == NULL)
		{
			printk("Failed to allocate message for CoAP Request\n");
			return;
		}
		otCoapMessageInit(myMessage, OT_COAP_TYPE_CONFIRMABLE, OT_COAP_CODE_PUT);
		error = otCoapMessageAppendUriPathOptions(myMessage, "sensors");
		if (error != OT_ERROR_NONE)
		{
			break;
		}
		error = otCoapMessageAppendContentFormatOption(myMessage, OT_COAP_OPTION_CONTENT_FORMAT_JSON);
		if (error != OT_ERROR_NONE)
		{
			break;
		}
		error = otCoapMessageSetPayloadMarker(myMessage);
		if (error != OT_ERROR_NONE)
		{
			break;
		}
		error = otMessageAppend(myMessage, myTemperatureJson, strlen(myTemperatureJson));
		if (error != OT_ERROR_NONE)
		{
			break;
		}
		memset(&myMessageInfo, 0, sizeof(myMessageInfo));
		memcpy(&myMessageInfo.mPeerAddr.mFields.m8[0], ml_prefix, 8);
		memcpy(&myMessageInfo.mPeerAddr.mFields.m8[8], serverInterfaceID, 8);
		myMessageInfo.mPeerPort = OT_DEFAULT_COAP_PORT;
		error = otIp6AddressFromString(serverIpAddr, &myMessageInfo.mPeerAddr);
		error = otCoapSendRequest(myInstance, myMessage, &myMessageInfo, coap_send_data_response_cb, NULL);
	} while (false);

	if (error != OT_ERROR_NONE)
	{
		printk("Failed to send CoAP Request: %d\n", error);
		otMessageFree(myMessage);
	}
	else
	{
		printk("CoAP data send.\n");
	}
}


