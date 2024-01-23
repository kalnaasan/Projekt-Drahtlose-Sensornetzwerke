#include <zephyr/net/openthread.h>
#include <openthread/thread.h>
#include <openthread/coap.h>
#include <stdio.h>
#include "sensor_functionality.h"

#define TEXTBUFFER_SIZE 256

#define STACK_SIZE 1024
#define SLEEP_TIME_SECONDS 60

/* CoAP */

const char *serverIpAddr = "fdda:72bc:8975:2:0:0:10.80.2.239";

void coap_init(void);
void coap_send_data_response_cb(void *p_context, otMessage *p_message, const otMessageInfo *p_message_info, otError result);
void coap_send_data_request(char *message);

/* Multi-Threading */

K_THREAD_STACK_DEFINE(sensor_stack, STACK_SIZE);
struct k_thread sensor_thread_data;

K_THREAD_STACK_DEFINE(coap_stack, STACK_SIZE);
struct k_thread coap_thread_data;

struct k_mutex data_mutex;
struct k_condvar data_ready_cv;

bool coap_data_ready = false;

void sensor_thread_entry(void) {
    while (1) {
        k_mutex_lock(&data_mutex, K_FOREVER);
        
		read_measurement();
		print_measurement();

        coap_data_ready = true;
        k_condvar_signal(&data_ready_cv);
        k_mutex_unlock(&data_mutex);

		k_sleep(K_SECONDS(SLEEP_TIME_SECONDS));
    }
}

void coap_thread_entry(void) {
    while (1) {
		k_mutex_lock(&data_mutex, K_FOREVER);
        
		while (!coap_data_ready) {
            // Wait for sensor data to be ready
            k_condvar_wait(&data_ready_cv, &data_mutex, K_FOREVER);
        }

        // Use shared_data for CoAP communication
        const char* my_sensor_data= (char*)malloc(TEXTBUFFER_SIZE * sizeof(char));
		my_sensor_data = create_coap_message();
		coap_send_data_request(my_sensor_data);
		free(my_sensor_data);

        coap_data_ready = false;
        k_mutex_unlock(&data_mutex);
    }
}



void main(void)
{
	int16_t error = 0;
	
	/* Init I2C, SC41, SVM41 */
	sensirion_i2c_hal_init();
	clean_up_sensor_states(error);

	/* Start periodic measurement */
	start_measurement(error);
	k_sleep(K_SECONDS(10));
	
	coap_init();

	k_mutex_init(&data_mutex);
    k_condvar_init(&data_ready_cv);

	k_tid_t sensor_thread_id = k_thread_create(&sensor_thread_data, sensor_stack, STACK_SIZE,
                                               sensor_thread_entry, NULL, NULL, NULL, 0, 
											   K_PRIO_COOP(0), K_NO_WAIT);

    k_tid_t coap_thread_id = k_thread_create(&coap_thread_data, coap_stack, STACK_SIZE,
                                             coap_thread_entry, NULL, NULL, NULL, 0, 
											 K_PRIO_COOP(0), K_NO_WAIT);

    k_thread_start(sensor_thread_id);
    k_thread_start(coap_thread_id);

    k_sleep(K_FOREVER);
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
