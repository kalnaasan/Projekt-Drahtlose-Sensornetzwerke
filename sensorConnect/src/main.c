#include <zephyr/smf.h>
#include <zephyr/net/openthread.h>
#include <openthread/thread.h>
#include <openthread/coap.h>
#include <stdio.h>

#include "sensor_functionality.h"

#define TEXTBUFFER_SIZE 256
#define SLEEP_TIME_SECONDS 60

/* CoAP */

const char *serverIpAddr = "fdda:72bc:8975:2:0:0:10.80.2.239";

void coap_init(void);
void coap_send_data_response_cb(void *p_context, otMessage *p_message, const otMessageInfo *p_message_info, otError result);
void coap_send_data_request(char *message);

/* SMF */

int32_t smf_sleep_sec = 0;

static const struct smf_state states[];

enum state { INIT, START_MEASUREMENT, READ_MEASUREMENT, SEND_DATA, IDLE_MODE};

struct s_object {
	/* This must be first */
	struct smf_ctx ctx;
	/* other state specific data add here */
	int16_t error;
} s_obj;

/* State INIT */
static void init_run(void *o){
	printk("INIT\n");

	struct s_object *s = (struct s_object *)o;
	s->error = 0;

	/* Init I2C, SC41, SVM41 */
	sensirion_i2c_hal_init();
	clean_up_sensor_states(s->error);

	/* Init CoAP */
	coap_init();

	smf_sleep_sec = 0;
	smf_set_state(SMF_CTX(&s_obj), &states[START_MEASUREMENT]);
}

/* State START_MEASUREMENT */
static void start_measurement_run(void *o){
	printk("START_MEASUREMENT\n");
	
	struct s_object *s = (struct s_object *)o;
	s->error = 0;
	
	/* Start periodic measurement */
	start_measurement(s->error);
	
	smf_sleep_sec = 10;
	smf_set_state(SMF_CTX(&s_obj), &states[READ_MEASUREMENT]);
}

/* State READ_MEASUREMENT */
static void read_measurement_run(void *o){
	printk("READ_MEASUREMENT\n");

	struct s_object *s = (struct s_object *)o;
	s->error = 0;

	read_measurement();
	print_measurement();

	smf_sleep_sec = 0;
	smf_set_state(SMF_CTX(&s_obj), &states[SEND_DATA]);
}

/* State SEND_DATA */
static void send_data_run(void *o){
	printk("SEND_DATA\n");
	
	// Use shared_data for CoAP communication
	const char* my_sensor_data= (char*)malloc(TEXTBUFFER_SIZE * sizeof(char));
	my_sensor_data = create_coap_message();
	coap_send_data_request(my_sensor_data);
	free(my_sensor_data);

	smf_sleep_sec = SLEEP_TIME_SECONDS;
	smf_set_state(SMF_CTX(&s_obj), &states[READ_MEASUREMENT]);
}

/* State STOP_MEASUREMENT */
static void stop_measurement_run(void *o){
	printk("STOP_MEASUREMENT\n");
	
	struct s_object *s = (struct s_object *)o;
	s->error = 0;
	
	/* Stop periodic measurement */
	stop_measurement(s->error);
	
	smf_sleep_sec = 10;
	smf_set_state(SMF_CTX(&s_obj), &states[READ_MEASUREMENT]);
}

/* State IDLE_MODE */
static void idle_mode_run(void *o){
	printk("IDLE_MODE\n");
	
	struct s_object *s = (struct s_object *)o;
	s->error = 0;
	
	/* */
	
	smf_sleep_sec = 0;
	smf_set_state(SMF_CTX(&s_obj), &states[IDLE_MODE]);
}

/* Populate state table */

static const struct smf_state states[] = {
	[INIT] = SMF_CREATE_STATE(NULL, init_run, NULL),
	[START_MEASUREMENT] = SMF_CREATE_STATE(NULL, start_measurement_run, NULL),
	[READ_MEASUREMENT] = SMF_CREATE_STATE(NULL, read_measurement_run, NULL),
	[SEND_DATA] = SMF_CREATE_STATE(NULL, send_data_run, NULL),
	[IDLE_MODE] = SMF_CREATE_STATE(NULL, idle_mode_run, NULL),
};

void main(void)
{
	int32_t ret;

	/* Set init state */
	smf_set_initial(SMF_CTX(&s_obj), &states[INIT]);
	
	while (1) {
		ret = smf_run_state(SMF_CTX(&s_obj));
		if(ret) {
			printk("Error: %d\n", ret);
			smf_set_initial(SMF_CTX(&s_obj), &states[INIT]);
			smf_sleep_sec = 60;
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
