#include <dk_buttons_and_leds.h>
#include <zephyr/smf.h>
#include <zephyr/net/openthread.h>
#include <openthread/thread.h>
#include <openthread/coap.h>
#include <zephyr/drivers/gpio.h>

#include "sensor_functionality.h"

#define SLEEP_TIME_FIVE 5
#define SLEEP_TIME_THIRTY 30
#define SLEEP_TIME_SIXTY 60
#define NO_SLEEP_TIME_SEC 0

#define NO_ERROR 0

#define NO_BLINKING_LED 5

/* Modes for Sensor Station */
static enum sensor_station_mode { 
	MEASURE, 
	CONFIG_PERIOD, 
	CALIBRATE
} station_mode;

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

static void stop_blinking_led() {

	k_timer_stop(&blinking_led_timer);
	dk_set_led_off(blinking_led);
}

static void activate_blinking_3_sec(struct k_work *item)
{
	//ARG_UNUSED(item);

	set_blinking_led();
	k_sleep(K_SECONDS(5));
	stop_blinking_led();
}


static void on_blinking_led_timer_expiry(struct k_timer *timer_id)
{
	static uint8_t val = 1;

	//ARG_UNUSED(timer_id);

	dk_set_led(blinking_led, val);
	val = !val;
}

static void on_blinking_led_timer_stop(struct k_timer *timer_id)
{
	//ARG_UNUSED(timer_id);

	dk_set_led_off(blinking_led);
}



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

/* CoAP */

const char *serverIpAddr = "fdda:72bc:8975:2:0:0:10.80.2.239";

void coap_init(void);
void coap_send_data_response_cb(void *p_context, otMessage *p_message, const otMessageInfo *p_message_info, otError result);
void coap_send_data_request(char *message);


/* SMF */

int32_t smf_sleep_sec = NO_SLEEP_TIME_SEC;

/* Forward declaration of state table */
static const struct smf_state states[];

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
	printk("INIT\n");

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
	smf_sleep_sec = 5;
	new_led_state = true;
	smf_set_state(SMF_CTX(&s_obj), &states[IDLE]);
}

const char* station_mode_to_string(enum sensor_station_mode s_mode) {
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
/* State IDLE */
static void idle_run(void *o){
	printk("IDLE\tMode: %s\n", station_mode_to_string(station_mode));
	
	struct s_object *s = (struct s_object *)o;
	s->error = NO_ERROR;
	
	/* Change states on Mode-Switch Button Press Event */
	switch(station_mode) {
		case CONFIG_PERIOD:
			if(btn2_pressed) {
				measure_period = FIVE;
				printk("Period Change to FIVE\n");
				new_led_state = true;
				btn2_pressed = false;
			}
			if(btn3_pressed) {
				measure_period = THIRTY;
				printk("Period Change to THIRTY\n");
				new_led_state = true;
				btn3_pressed = false;
			}
			if(btn4_pressed) {
				measure_period = SIXTY;
				printk("Period Change to SIXTY\n");
				new_led_state = true;
				btn4_pressed = false;
			}
			if(new_led_state) {
				dk_set_leds(DK_NO_LEDS_MSK);
				dk_set_led_on(DK_LED2);
				set_blinking_led();
				new_led_state = false;
			}
			break;

		case CALIBRATE:
			if(btn2_pressed) {
				if(calib_mode == CO2 && run_calibration) {
					smf_set_state(SMF_CTX(&s_obj), &states[START_CALIB_MEASUREMENT]);
					smf_sleep_sec = 1;
					btn2_pressed = false;
					new_led_state = true;
					return;
				}
				else if(calib_mode == CO2) {
					printk("-> selected CO2\n");
					run_calibration = true;
					set_select_led();
				}
				else {
					printk("Change Calib to CO2\n");
					calib_mode = CO2;
					run_calibration = false;
					new_led_state = true;
				}
				btn2_pressed = false;
			}
			if(btn3_pressed) {
				if(calib_mode == VOC && run_calibration) {
					smf_set_state(SMF_CTX(&s_obj), &states[START_CALIB_MEASUREMENT]);
					smf_sleep_sec = 1;
					btn3_pressed = false;
					new_led_state = true;
					return;
				}
				else if(calib_mode == VOC) {
					printk("-> selected VOC\n");
					run_calibration = true;
					set_select_led();
				}
				else {
					printk("Change Calib to VOC\n");
					calib_mode = VOC;
					run_calibration = false;
					new_led_state = true;
				}
				btn3_pressed = false;
			}
			if(btn4_pressed) {
				if(calib_mode == TEMP && run_calibration) {
					smf_set_state(SMF_CTX(&s_obj), &states[START_CALIB_MEASUREMENT]);
					smf_sleep_sec = 1;
					btn4_pressed = false;
					new_led_state = true;
					return;
				}
				else if(calib_mode == TEMP) {
					printk("-> selected TEMP\n");
					run_calibration = true;
					set_select_led();
				
				}
				else {
					printk("Change Calib to TEMP\n");
					calib_mode = TEMP;
					run_calibration = false;
					new_led_state = true;
				}
				btn4_pressed = false;
			}
			if(new_led_state) {
				dk_set_leds(DK_NO_LEDS_MSK);
				dk_set_led_on(DK_LED3);
				set_blinking_led();
				new_led_state = false;
			}
			break;

		case MEASURE:
		default:	 		
			if(btn2_pressed) {
				if(run_per_measurement) {
					smf_set_state(SMF_CTX(&s_obj), &states[START_MEASUREMENT]);
					smf_sleep_sec = 1;
					btn2_pressed = false;
					return;
				}
				else {
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
				dk_set_led_on(DK_LED1);
				set_blinking_led();
				new_led_state = false;
			}
	}
	
	smf_sleep_sec = 1;
	smf_set_state(SMF_CTX(&s_obj), &states[IDLE]);
}

/* State START_MEASUREMENT */
static void start_measurement_run(void *o){
	printk("START_MEASUREMENT\n");

	struct s_object *s = (struct s_object *)o;
	s->error = NO_ERROR;

	dk_set_leds(DK_NO_LEDS_MSK);
	stop_blinking_led();
	blinking_led = DK_LED1;
	
	/* Start periodic measurement */
	start_periodic_measurement(&(s->error));

	smf_sleep_sec = 10;
	smf_set_state(SMF_CTX(&s_obj), &states[READ_MEASUREMENT]);
}

/* State STOP_MEASUREMENT */
static void stop_measurement_run(void *o){
	printk("STOP_MEASUREMENT\n");
	
	struct s_object *s = (struct s_object *)o;
	s->error = NO_ERROR;

	stop_blinking_led();

	/* Stop periodic measurement */
	stop_periodic_measurement(&(s->error));

	dk_set_leds(DK_ALL_LEDS_MSK);

	k_msleep(500);
	
	new_led_state = true;
	smf_sleep_sec = NO_SLEEP_TIME_SEC;
	smf_set_state(SMF_CTX(&s_obj), &states[IDLE]);
}

/* State READ_MEASUREMENT */
static void read_measurement_run(void *o){
	printk("READ_MEASUREMENT\n");

	struct s_object *s = (struct s_object *)o;
	s->error = NO_ERROR;

	stop_blinking_led();

	read_measurement(&(s->error));
	print_measurement();

	smf_sleep_sec = 0;
	smf_set_state(SMF_CTX(&s_obj), &states[SEND_DATA]);
}

/* State SEND_DATA */
static void send_data_run(void *o){
	printk("SEND_DATA\n");

	struct s_object *s = (struct s_object *)o;
	s->error = NO_ERROR;
	
	// Use shared_data for CoAP communication
	char* my_sensor_data;
	my_sensor_data = create_coap_message();
	coap_send_data_request(my_sensor_data);

	k_work_submit(&blinking_led_3sec_work);

	free(my_sensor_data);

	smf_sleep_sec = SLEEP_TIME_FIVE;
	smf_set_state(SMF_CTX(&s_obj), &states[READ_MEASUREMENT]);
}

/* State START_CALIB_MEASUREMENT */
static void start_calib_measurement_entry(void *o) {
	printk("START_CALIB_MEASUREMENT\n");// do sth
}

static void start_calib_measurement_run(void *o){
	
	
	struct s_object *s = (struct s_object *)o;
	s->error = NO_ERROR;

	set_select_led();
	switch(calib_mode) {
		case CO2:
			/**
			 * Measure for 3 Minutes CO2
			 * Check every 5 seconds if stop Button has been pressed
			 * 36 * 5 sec = 180 sec = 3 Min
			*/
			start_periodic_measurement(&(s->error));
			if(s->error) return;

			for(uint16_t i = 0; i < 36; i++) {
				k_sleep(K_SECONDS(SLEEP_TIME_FIVE));	// 5 sec
				if(btn2_pressed) {					
					smf_set_state(SMF_CTX(&s_obj), &states[STOP_MEASUREMENT]);
					smf_sleep_sec = 1;
					btn2_pressed = false;
					return;
				}
			}
			stop_periodic_measurement(&(s->error));
			stop_blinking_led();
			smf_sleep_sec = 1;
			break;
		case VOC:
			break;
		case TEMP:
			break;
	}
	
	
	smf_set_state(SMF_CTX(&s_obj), &states[CALIB_READY]);
}

/* State CALIB_READY */
static void calib_ready_entry(void *o) {
	printk("CALIB_READY\n");// do sth
}

static void calib_ready_run(void *o){
	
	
	struct s_object *s = (struct s_object *)o;
	s->error = NO_ERROR;
	set_blinking_led();
	switch(calib_mode) {
		case CO2:
			// LED shows calib_ready
			if(btn2_pressed) {
				if(run_frc) {
					smf_set_state(SMF_CTX(&s_obj), &states[FRC]);
					smf_sleep_sec = NO_SLEEP_TIME_SEC;
					btn2_pressed = false;
					return;
				}
				run_frc = true;
				set_select_led();
				break;
			}
			break;
		case VOC:
			break;
		case TEMP:
		default:
	}
	
	smf_sleep_sec = NO_SLEEP_TIME_SEC;
	smf_set_state(SMF_CTX(&s_obj), &states[CALIB_READY]);
}

/* State FRC */
static void frc_entry(void *o) {
	printk("FRC\n");// do sth
}

static void frc_run(void *o){
	
	
	struct s_object *s = (struct s_object *)o;
	s->error = NO_ERROR;
	stop_blinking_led();

	switch(calib_mode) {
		case CO2:
			forced_co2_recalibration(&(s->error));
			break;
		case VOC:
			break;
		case TEMP:
		default:
	}
	
	smf_sleep_sec = 1;
	run_frc = false;
	new_led_state = true;
	smf_set_state(SMF_CTX(&s_obj), &states[IDLE]);
}

/* Populate state table */
static const struct smf_state states[] = {
	[INIT] = SMF_CREATE_STATE(NULL, init_run, NULL),
	[IDLE] = SMF_CREATE_STATE(NULL, idle_run, NULL),
	[START_MEASUREMENT] = SMF_CREATE_STATE(NULL, start_measurement_run, NULL),
	[STOP_MEASUREMENT] = SMF_CREATE_STATE(NULL, stop_measurement_run, NULL),
	[READ_MEASUREMENT] = SMF_CREATE_STATE(NULL, read_measurement_run, NULL),
	[SEND_DATA] = SMF_CREATE_STATE(NULL, send_data_run, NULL),
	[START_CALIB_MEASUREMENT] = SMF_CREATE_STATE(start_calib_measurement_entry, start_calib_measurement_run, NULL),
	[CALIB_READY] = SMF_CREATE_STATE(calib_ready_entry, calib_ready_run, NULL),
	[FRC] = SMF_CREATE_STATE(frc_entry, frc_run, NULL)
};


int main(void)
{
	int32_t ret;

	k_timer_init(&blinking_led_timer, on_blinking_led_timer_expiry, on_blinking_led_timer_stop);
	
	k_work_init(&blinking_led_3sec_work, activate_blinking_3_sec);

	ret = dk_leds_init();
	if (ret) {
		//LOG_ERR("Could not initialize leds, err code: %d", ret);
		goto end;
	}

	ret = dk_buttons_init(on_button_changed);
	if (ret) {
		//LOG_ERR("Cannot init buttons (error: %d)", ret);
		goto end;
	}

	/* Set init state */
	smf_set_initial(SMF_CTX(&s_obj), &states[INIT]);
	
	while (1) {
		ret = smf_run_state(SMF_CTX(&s_obj));
		if(ret) {
			printk("SMF Run State Error: %d\n", ret);
			smf_set_initial(SMF_CTX(&s_obj), &states[INIT]);
			smf_sleep_sec = 10;
			continue;
		}
		if(mode_switch) {
			switch(station_mode) {
				case CONFIG_PERIOD: 
					station_mode = CALIBRATE;
					smf_sleep_sec = NO_SLEEP_TIME_SEC;
					smf_set_state(SMF_CTX(&s_obj), &states[IDLE]);
					new_led_state = true;
					break;

				case CALIBRATE: 	
					station_mode = MEASURE;
					run_frc = false;
					smf_sleep_sec = NO_SLEEP_TIME_SEC;
					(run_calibration || run_frc) ? (SMF_CTX(&s_obj), &states[STOP_MEASUREMENT])
									: (SMF_CTX(&s_obj), &states[IDLE]);
					new_led_state = true;
					break;

				case MEASURE:
				default:	 		
					station_mode = CONFIG_PERIOD;
					smf_sleep_sec = NO_SLEEP_TIME_SEC;
					run_per_measurement ? (SMF_CTX(&s_obj), &states[STOP_MEASUREMENT])
										: (SMF_CTX(&s_obj), &states[IDLE]);
					new_led_state = true;
			};
			mode_switch = false;
			new_led_state = true;
		}
		k_sleep(K_SECONDS(smf_sleep_sec));
	}
end:
	return 0;
}

void coap_init(void)
{
	otInstance *p_instance = openthread_get_default_instance();
	otError error = otCoapStart(p_instance, OT_DEFAULT_COAP_PORT);
	if (error != OT_ERROR_NONE)
	{
		printk("Failed to start Coap: %d\n", error);
	}
}

void coap_send_data_response_cb(void *p_context, otMessage *p_message, const otMessageInfo *p_message_info, otError result)
{
	if (result == OT_ERROR_NONE)
	{
		printk("Delivery confirmed.\n");
	}
	else
	{
		printk("Delivery not confirmed: %d\n", result);
	}
}

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
