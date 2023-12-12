#include <zephyr/net/openthread.h>
#include <openthread/thread.h>
#include <openthread/coap.h>
//#include <zephyr/data/json.h>
#include <stdio.h>

#include "sensor_functionality.h"

#define SLEEP_TIME_MS 1000
#define TEXTBUFFER_SIZE 256

/* CoAP Module */
static void coap_send_data_request(void);
static void coap_send_data_response_cb(void * p_context, otMessage * p_message,
										const otMessageInfo * p_message_info, otError result);
void coap_init(void);

void main(void){
	int16_t error = 0;

	/* Init I2C, SC41, SVM41 */
	sensirion_i2c_hal_init();
	clean_up_sensor_states(error);
	s_state = SENSOR_INIT;

	/* Start periodic measurement */
	start_measurement(error);
	s_state = PERIODIC_MEASURING;
	
	coap_init();
	
	k_msleep(5000); // for safety
	
	while(1) {
		/* Read Measurement */
		read_measurement();
		print_measurement();
		/* CoAP Message*/
		coap_send_data_request();
		k_msleep(5000);
	}
}

static void coap_send_data_response_cb(void * p_context, otMessage * p_message,
										const otMessageInfo * p_message_info, otError result){
	if (result == OT_ERROR_NONE) {
		printk("Delivery confirmed.\n");
	} else {
		printk("Delivery not confirmed: %d\n", result);
	}
}

static void coap_send_data_request(void){
	otError error = OT_ERROR_NONE;
	otMessage * myMessage;
	otMessageInfo myMessageInfo;
	otInstance * myInstance = openthread_get_default_instance();
	
	const otMeshLocalPrefix *ml_prefix = otThreadGetMeshLocalPrefix(myInstance);
	uint8_t serverInterfaceID[8]= {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01};
	
	if(s_state != PERIODIC_MEASURING) {
		if(!valid_SC41_data || !valid_SVM41_data) {
			printk("Data not ready for transmitting via CoAP.");
			error = -20;
			goto exit;
		}
	}

	const char* my_sensor_data = create_coap_message();
	printk("SCD41 co2: %u", SCD41_co2);
	const char jsonBuffer[TEXTBUFFER_SIZE];

    // Build the JSON string manually
    int ret = snprintk(jsonBuffer, sizeof(jsonBuffer),
                       "{\"SCD41\":{\"co2\":%u,\"temp\":%d,\"hum\":%d},\"SVM41\":{\"hum\":%i,\"temp\":%i,\"voc\":%i,\"nox\":%i}}",
                       SCD41_co2, SCD41_temperature, SCD41_humidity, SVM41_humidity * 10, (SVM41_temperature >> 1) * 10, SVM41_voc_index, SVM41_nox_index);

    if (ret >= 0 && ret < sizeof(jsonBuffer))
    {
        // Print the JSON Encoded Data to the console
        printk("JSON Encoded Data Inside: %s\n", jsonBuffer);
    }
    else
    {
        printk("JSON Encoding Failed: %d\n", ret);
    }
	return jsonBuffer;
    printk("JSON Encoded Data Outside: %s\n", my_sensor_data);
	do{
		myMessage = otCoapNewMessage(myInstance, NULL);
		if (myMessage == NULL) {
			printk("Failed to allocate message for CoAP Request\n"); return;
		}

		otCoapMessageInit(myMessage, OT_COAP_TYPE_CONFIRMABLE, OT_COAP_CODE_PUT);
		
		error = otCoapMessageAppendUriPathOptions(myMessage, "storedata");
		if (error != OT_ERROR_NONE){ break; }
		
		error = otCoapMessageAppendContentFormatOption(myMessage,
											OT_COAP_OPTION_CONTENT_FORMAT_JSON );
		if (error != OT_ERROR_NONE){ break; }

		error = otCoapMessageSetPayloadMarker(myMessage);
		if (error != OT_ERROR_NONE){ break; }

		error = otMessageAppend(myMessage, my_sensor_data,
		strlen(my_sensor_data));
		if (error != OT_ERROR_NONE){ break; }

		memset(&myMessageInfo, 0, sizeof(myMessageInfo));
		memcpy(&myMessageInfo.mPeerAddr.mFields.m8[0], ml_prefix, 8);
		memcpy(&myMessageInfo.mPeerAddr.mFields.m8[8], serverInterfaceID, 8);
		myMessageInfo.mPeerPort = OT_DEFAULT_COAP_PORT;

		
		error = otCoapSendRequest(myInstance, myMessage, &myMessageInfo,
											coap_send_data_response_cb, NULL);
	}while(false);

	exit:
		if (error != OT_ERROR_NONE) {
			printk("Failed to send CoAP Request: %d\n", error);
			otMessageFree(myMessage);
		}else{
			printk("CoAP data send.\n");
		}
}

void coap_init(void){
	otInstance * p_instance = openthread_get_default_instance();
	otError error = otCoapStart(p_instance, OT_DEFAULT_COAP_PORT);
	if (error!=OT_ERROR_NONE)
		printk("Failed to start Coap: %d\n", error);
}