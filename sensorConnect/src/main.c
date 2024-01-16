#include <zephyr/net/openthread.h>
#include <openthread/thread.h>
#include <openthread/coap.h>
// #include <zephyr/data/json.h>
#include <stdio.h>
#include "sensor_functionality.h"

#define SLEEP_TIME_MS 1000
#define TEXTBUFFER_SIZE 256

const char *serverIpAddr = "fdda:72bc:8975:2:0:0:10.80.2.239";

void coap_init(void);
void coap_send_data_response_cb(void *p_context, otMessage *p_message, const otMessageInfo *p_message_info, otError result);
void coap_send_data_request(char *message);

void main(void)
{
	int16_t error = 0;
	/*
	SCD41_co2 = 1135;
	SCD41_temperature = 23431;
	SCD41_humidity = 44123;
	SVM41_humidity = 39600;
	SVM41_temperature = 23880;
	SVM41_voc_index = 325;
	SVM41_nox_index = 18;
	*/

	/* Init I2C, SC41, SVM41 */
	sensirion_i2c_hal_init();
	clean_up_sensor_states(error);
	s_state = SENSOR_INIT;

	/* Start periodic measurement */
	start_measurement(error);
	s_state = PERIODIC_MEASURING;
	coap_init();

	k_msleep(SLEEP_TIME_MS); // for safety

	while (true)
	{
		/* Read Measurement */
		read_measurement();
		print_measurement();
		const char* my_sensor_data= (char*)malloc(TEXTBUFFER_SIZE * sizeof(char));
		my_sensor_data = create_coap_message();
		coap_send_data_request(my_sensor_data);
		free(my_sensor_data);
		k_msleep(5000);
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
