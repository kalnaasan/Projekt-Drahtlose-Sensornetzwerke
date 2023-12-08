#include <zephyr/net/openthread.h>
#include <openthread/thread.h>
#include <openthread/coap.h>

//#include "sensor_functionality.h"
#include "scd4x_i2c.h"
#include "svm41_i2c.h"
#include "sensirion_common.h"
#include "sensirion_i2c_hal.h"

typedef enum{
    SLEEP,
	SENSOR_INIT,
	PERIODIC_MEASURING,   
} sensor_state;

static sensor_state s_state = SLEEP;

/* for now, data is stored like this, will be omitted later*/
static uint16_t SC41_co2;
static int32_t SCD41_temperature;
static int32_t SCD41_humidity;

static int16_t SVM41_humidity;
static int16_t SVM41_temperature;
static int16_t SVM41_voc_index;
static int16_t SVM41_nox_index;


static bool valid_SC41_data = false;
static bool valid_SVM41_data = false;


/* Multithreading */
#define THREAD0_STACKSIZE 512
#define THREAD1_STACKSIZE 512
#define THREAD0_PRIORITY 4 
#define THREAD1_PRIORITY 4
K_MUTEX_DEFINE(test_mutex);


void shared_code_section(void){
	/* Lock the mutex */
	k_mutex_lock(&test_mutex, K_FOREVER);
	/* Do something */
	/* Unlock the mutex */
	k_mutex_unlock(&test_mutex);
}

/* Sensor functionality */
void clean_up_sensor_states(int16_t* error) {
	// SCD41
	scd4x_wake_up();
	scd4x_stop_periodic_measurement();
    error = scd4x_reinit();
    if (error) {
        printk("Error executing scd4x_reinit(): %i\n", error);
    }
	// SVM41
	error = svm41_device_reset();
    if (error) {
        printk("Error executing svm41_device_reset(): %i\n", error);
    }
}

void start_measurement(int16_t* error) {
	// SCD41
	error = scd4x_start_periodic_measurement();
    if (error) {
        printk("Error executing scd4x_start_periodic_measurement(): %i\n",
               error);
    }
	// SVM41
	error = svm41_start_measurement();
    if (error) {
        printk("Error executing svm41_start_measurement(): %i\n", error);
    }
}

/**
	 * example JSON file:
	 * 
	 * [{"sensor": SCD41, "co2": <value>, "temperature": <value>, "humidity": <value>},
	 * {"sensor": SVM41, "humidity": <value>, "temperature": <value>, "voc": <value>}, "nox": <value>]
	 * 
	 * To Do: Encode data into JSON file.
	 * 
	*/

char* data_to_json() {
	// todo.
	return "{\"sensor\": SCD41, \"co2\": <value>, \"temperature\": <value>, \"humidity\": <value>}, {\"sensor\": SVM41, \"humidity\": <value>, \"temperature\": <value>, \"voc\": <value>, \"nox\": <value>}";

}

void sensor_reading_thread(void)
{
	printk("sensor_reading_thread started\n");

	int16_t error = 0;

	/* Init I2C, SC41, SVM41 */
	sensirion_i2c_hal_init();
	clean_up_sensor_states(error);
	s_state = SENSOR_INIT;

	/* Start periodic measurement */
	start_measurement(error);
	s_state = PERIODIC_MEASURING;

	while (1) {
		printk("->\t New Reading of Sensor Node\n");
		/* Read Measurement */
		sensirion_i2c_hal_sleep_usec(1000000*2);	// 2 sec

		// SCD41
		bool data_ready_flag = false;
		error = scd4x_get_data_ready_flag(&data_ready_flag);
		if (error) {
			printk("Error executing scd4x_get_data_ready_flag(): %i\n", error);
			continue;
		}
		if (!data_ready_flag) {
			printk("SCD41 Data not ready\n");
			continue;
		}
		valid_SC41_data = false;
		error = scd4x_read_measurement(&SC41_co2, &SCD41_temperature, &SCD41_humidity);
		if (error) {
			printk("Error executing scd4x_read_measurement(): %i\n", error);
			continue;
		} else if (SC41_co2 == 0) {
			printk("Invalid SCD41 sample detected, skipping this sensor.\n");
		} else {
			valid_SC41_data = true;
		}
		// SVM41
		valid_SVM41_data = false;
		error = svm41_read_measured_values_as_integers(&SVM41_humidity, &SVM41_temperature,
													&SVM41_voc_index, &SVM41_nox_index);
		if (error) {
			printk("Error executing svm41_read_measured_values_as_integers(): %i\n",error);
			continue;
		} else {
			valid_SVM41_data = true;
		}

		/* Print Measurements */
			printk("->\tMeasurements\n");
			printk("  SCD41 Measurement\n");
			if(valid_SC41_data) {
				
				printk("CO2: %u\n", SC41_co2);
				printk("Temperature: %d m°C\n", SCD41_temperature);
				printk("Humidity: %d mRH\n", SCD41_humidity);
			}else(printk("No Valid Data.\n"));
			
			printk("  SVM41 Measurement\n");
			if(valid_SVM41_data) {
				printk("Humidity: %i milli %% RH\n", SVM41_humidity * 10);
				printk("Temperature: %i milli °C\n", (SVM41_temperature >> 1) * 10);
				printk("VOC index: %i (index * 10)\n", SVM41_voc_index);
				printk("NOx index: %i (index * 10)\n", SVM41_nox_index);
			}
			printk("\n");
	}
}

/* Space for creating CoAP Message from Data*/
#define TEXTBUFFER_SIZE 180
char my_JSON[TEXTBUFFER_SIZE];

/* CoAP Module */

void coap_init(void){
	otInstance * p_instance = openthread_get_default_instance();
	otError error = otCoapStart(p_instance, OT_DEFAULT_COAP_PORT);
	if (error!=OT_ERROR_NONE)
		printk("Failed to start Coap: %d\n", error);
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
			goto exit;
		}
	}

	const char * mySensorDataJson = data_to_json();

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

		error = otMessageAppend(myMessage, mySensorDataJson,
		strlen(mySensorDataJson));
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



void coap_thread(void)
{
	coap_init();

	k_msleep(5000); // for safety

	while(1) {	
		coap_send_data_request();
		k_msleep(5000);
	}
}

/**
 * Easy fix for sensor reading.
 * This Multi-Thread (not Openthread) starts the periodic measurement of sensor data and saves it every 2 seconds.
 * Should be adjustable so the sensors would sleep as well.
 * 
 * For now, the data might be overwritten before it is send to the server via OT.
*/
K_THREAD_DEFINE(sensor_reading_thread_id, THREAD0_STACKSIZE, sensor_reading_thread, NULL, NULL, NULL,
		THREAD0_PRIORITY, 0, 5000);

K_THREAD_DEFINE(coap_thread_id, THREAD1_STACKSIZE, coap_thread, NULL, NULL, NULL,
		THREAD1_PRIORITY, 0, 10000);