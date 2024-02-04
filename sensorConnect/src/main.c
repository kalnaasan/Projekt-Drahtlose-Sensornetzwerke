#include <zephyr/smf.h>
#include <zephyr/net/openthread.h>
#include <openthread/thread.h>
#include <openthread/coap.h>
#include <stdio.h>
#include <zephyr/drivers/gpio.h>

#include "sensor_functionality.h"

#define SLEEP_TIME_FIVE 5
#define SLEEP_TIME_THIRTY 30
#define SLEEP_TIME_SIXTY 60
#define NO_SLEEP_TIME_SEC 0



#define NO_ERROR 0

#define BUTTON0_NODE DT_NODELABEL(button0) //DT_N_S_buttons_S_button_0
#define BUTTON1_NODE DT_NODELABEL(button1) //DT_N_S_buttons_S_button_1
#define BUTTON2_NODE DT_NODELABEL(button2) //DT_N_S_buttons_S_button_2
#define BUTTON3_NODE DT_NODELABEL(button3) //DT_N_S_buttons_S_button_3

/* Heap Allocation for JSON String (CoAP Message)*/
K_HEAP_DEFINE(text_buffer_heap, 512);

/* Modes for Sensor Station */
static enum sensor_station_mode { 
	MEASURE, 
	CONFIG_PERIOD, 
	CALIBRATE
} station_mode;

static bool mode_switch;
static bool calibration_selected;
static bool run_per_measurement;
static bool run_frc;
/* List of events */
#define EVENT_BTN1_PRESS BIT(0)
#define EVENT_BTN2_PRESS BIT(1)
#define EVENT_BTN3_PRESS BIT(2)

/* Buttons*/
static const struct gpio_dt_spec button0_spec = GPIO_DT_SPEC_GET(BUTTON0_NODE, gpios);
static struct gpio_callback button0_cb;



static const struct gpio_dt_spec button1_spec = GPIO_DT_SPEC_GET(BUTTON1_NODE, gpios);
static struct gpio_callback button1_cb;



static const struct gpio_dt_spec button2_spec = GPIO_DT_SPEC_GET(BUTTON2_NODE, gpios);
static struct gpio_callback button2_cb;



static const struct gpio_dt_spec button3_spec = GPIO_DT_SPEC_GET(BUTTON3_NODE, gpios);
static struct gpio_callback button3_cb;



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
	
	/* Events */
	struct k_event smf_event;
	int32_t events;

	int16_t error;
} s_obj;

/* State INIT */
static void init_run(void *o){
	printk("INIT\n");

	struct s_object *s = (struct s_object *)o;
	s->error = NO_ERROR;

	/* Init Settings for Sensor Station */
	station_mode = MEASURE;
	mode_switch = false;

	calib_mode = CO2;
	calibration_selected = false;
	run_frc = false;
		
	measure_period = FIVE;
	run_per_measurement = false;



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

	smf_sleep_sec = 5;
	smf_set_state(SMF_CTX(&s_obj), &states[IDLE]);
}

/* State IDLE */
static void idle_run(void *o){
	printk("IDLE\n");
	
	struct s_object *s = (struct s_object *)o;
	s->error = NO_ERROR;
	
	/* Change states on Mode-Switch Button Press Event */
	switch(station_mode) {
		case CONFIG_PERIOD:
			if(s->events & EVENT_BTN1_PRESS) measure_period = FIVE;
			if(s->events & EVENT_BTN2_PRESS) measure_period = THIRTY;
			if(s->events & EVENT_BTN3_PRESS) measure_period = SIXTY;
			break;

		case CALIBRATE:
			if(s->events & EVENT_BTN1_PRESS) {
				if(calib_mode == CO2 && calibration_selected) {
					smf_set_state(SMF_CTX(&s_obj), &states[START_CALIB_MEASUREMENT]); //CHANGE IT!!!!!!!!!!!!!!!!!!!!
					smf_sleep_sec = 1;
					return;
				}
				else if(calib_mode == CO2) {
					calibration_selected = true;
				} // Start calibration
				else {
					calib_mode = CO2;
					calibration_selected = false;
				}
			}
			if(s->events & EVENT_BTN2_PRESS) {
				if(calib_mode == VOC && calibration_selected) {
					smf_set_state(SMF_CTX(&s_obj), &states[START_CALIB_MEASUREMENT]); //CHANGE IT!!!!!!!!!!!!!!!!!!!!
					smf_sleep_sec = 1;
					return;
				}
				else if(calib_mode == VOC) {
					calibration_selected = true;
				} // Start calibration
				else {
					calib_mode = VOC;
					calibration_selected = false;
				}
			}
			if(s->events & EVENT_BTN3_PRESS) {
				if(calib_mode == TEMP && calibration_selected) {
					smf_set_state(SMF_CTX(&s_obj), &states[START_CALIB_MEASUREMENT]); //CHANGE IT!!!!!!!!!!!!!!!!!!!!
					smf_sleep_sec = 1;
					return;
				}
				else if(calib_mode == TEMP) {
					calibration_selected = true;
				} // Start calibration
				else {
					calib_mode = TEMP;
					calibration_selected = false;
				}
			}
			break;

		case MEASURE:
		default:	 		
			if(s->events & EVENT_BTN1_PRESS) {
				if(run_per_measurement) {
					smf_set_state(SMF_CTX(&s_obj), &states[START_MEASUREMENT]);
					smf_sleep_sec = 1;
					return;
				}
				else {
					run_per_measurement = true;
				}
			}
			if(s->events & EVENT_BTN2_PRESS) {
				// do nothing
			}
			if(s->events & EVENT_BTN3_PRESS) {
				// do nothing
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
	
	/* Stop periodic measurement */
	stop_periodic_measurement(&(s->error));
	k_msleep(500);
	smf_sleep_sec = NO_SLEEP_TIME_SEC;
	smf_set_state(SMF_CTX(&s_obj), &states[IDLE]);
}

/* State READ_MEASUREMENT */
static void read_measurement_run(void *o){
	printk("READ_MEASUREMENT\n");

	struct s_object *s = (struct s_object *)o;
	s->error = NO_ERROR;

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
	my_sensor_data = create_coap_message(&text_buffer_heap, &(s->error));
	coap_send_data_request(my_sensor_data);
	k_heap_free(&text_buffer_heap, my_sensor_data);

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
	switch(calib_mode) {
		case CO2:
			/**
			 * Measure for 3 Minutes CO2
			 * Check every 5 seconds if stop Button has been pressed
			 * 36 * 5 sec = 180 sec = 3 Min
			*/
			start_periodic_measurement(&(s->error));
			if(s->error) return;

			for(u_int16_t i = 0; i < 36; i++) {
				k_sleep(K_SECONDS(SLEEP_TIME_FIVE));	// 5 sec
				if(s->events & EVENT_BTN2_PRESS) {					
					smf_set_state(SMF_CTX(&s_obj), &states[STOP_MEASUREMENT]);
					smf_sleep_sec = 1;
					return;
				}
			}
			stop_periodic_measurement(&(s->error));
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
	switch(calib_mode) {
		case CO2:
			// LED shows calib_ready
			if(s->events & EVENT_BTN2_PRESS) {
				if(run_frc) {
					smf_set_state(SMF_CTX(&s_obj), &states[FRC]);
					smf_sleep_sec = NO_SLEEP_TIME_SEC;
					return;
				}
				run_frc = true;
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

/* Button Handler*/
void button0_callback(const struct device *gpiob, struct gpio_callback *cb,
                                     gpio_port_pins_t pins){
	mode_switch = true;
}
void button1_callback(const struct device *gpiob, struct gpio_callback *cb,
                                     gpio_port_pins_t pins){
    /* Generate Button Press Event */
    k_event_post(&s_obj.smf_event, EVENT_BTN1_PRESS);
    
}
void button2_callback(const struct device *gpiob, struct gpio_callback *cb,
                                     gpio_port_pins_t pins){
    /* Generate Button Press Event */
    k_event_post(&s_obj.smf_event, EVENT_BTN2_PRESS);
    
}
void button3_callback(const struct device *gpiob, struct gpio_callback *cb,
                                     gpio_port_pins_t pins){
    /* Generate Button Press Event */
    k_event_post(&s_obj.smf_event, EVENT_BTN3_PRESS);
    
}

void main(void)
{
	int32_t ret;

	if (!gpio_is_ready_dt(&button0_spec)) {
		printk("Error: button device %s is not ready\n",
				button0_spec.port->name);
		return;
	}

	//init button(s)
	gpio_pin_configure_dt(&button0_spec, GPIO_INPUT);

    gpio_pin_interrupt_configure_dt(&button0_spec, GPIO_INT_EDGE_TO_ACTIVE);
    gpio_init_callback(&button0_cb, button0_callback, BIT(button0_spec.pin) );
    gpio_add_callback(button0_spec.port, &button0_cb);

	

	/* Initialize the event */
    k_event_init(&s_obj.smf_event);

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
					break;

				case CALIBRATE: 	
					station_mode = MEASURE;
					run_frc = false;
					smf_sleep_sec = NO_SLEEP_TIME_SEC;
					smf_set_state(SMF_CTX(&s_obj), &states[STOP_MEASUREMENT]);
					break;

				case MEASURE:
				default:	 		
					station_mode = CONFIG_PERIOD;
					smf_sleep_sec = NO_SLEEP_TIME_SEC;
					smf_set_state(SMF_CTX(&s_obj), &states[STOP_MEASUREMENT]);
			};
			mode_switch = false;
		}
		k_sleep(K_SECONDS(smf_sleep_sec));
	}
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
