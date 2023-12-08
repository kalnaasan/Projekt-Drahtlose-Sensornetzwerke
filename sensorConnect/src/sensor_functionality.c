//#include <zephyr/data/json.h>

#include "sensor_functionality.h"
#include "scd4x_i2c.h"
#include "svm41_i2c.h"
#include "sensirion_common.h"
#include "sensirion_i2c_hal.h"

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
