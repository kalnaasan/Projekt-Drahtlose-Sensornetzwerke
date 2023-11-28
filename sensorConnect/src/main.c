#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/sys/printk.h>

#include "scd4x_i2c.h"
#include "svm41_i2c.h"
#include "sgp41_i2c.h"
#include "sensirion_common.h"
#include "sensirion_i2c_hal.h"

void printErr(int err, char* errType, uint8_t* buffer) {
	printk("ERR = %d |\t Type: %s, Buffer = [%d, %d] -> HEX %#2x%2x\n", err, errType, buffer[0], buffer[1], buffer[0], buffer[1]);
}
void printSuc(char* errType, uint8_t* buffer) {
	printk("Command-> %s |\t Buffer = [%d, %d] -> HEX %#2x%2x\n", errType, buffer[0], buffer[1], buffer[0], buffer[1]);
}

void clean_up_sensor_states(int16_t* error) {
	// SCD41
	scd4x_wake_up();
	scd4x_stop_periodic_measurement();
	scd4x_reinit();
	// SVM41
	error = svm41_device_reset();
    if (error) {
        printk("Error executing svm41_device_reset(): %i\n", error);
    }
	// SGP41 - NOT NEEDED?
}

void self_test_SGP41(int16_t* error) {

	uint16_t test_result;

	error = sgp41_execute_self_test(&test_result);
    if (error) {
        printk("Error executing sgp41_execute_self_test(): %i\n", error);
    } else {
        printk("Test result: %u\n", test_result);
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
	// SGP41
		// Parameters for deactivated SCD41_humidity compensation:
    uint16_t default_rh = 0x8000;
    uint16_t default_t = 0x6666;

    	// sgp41 conditioning during 10 seconds before measuring
    for (int i = 0; i < 10; i++) {
        uint16_t sraw_voc;

        sensirion_i2c_hal_sleep_usec(1000000);

        error = sgp41_execute_conditioning(default_rh, default_t, &sraw_voc);
        if (error) {
            printk("Error executing sgp41_execute_conditioning(): "
                   "%i\n",
                   error);
        } else {
            printk("SRAW VOC: %u\n", sraw_voc);
            printk("SRAW NOx: conditioning\n");
        }
    }
}


void main(void){

	int16_t error = 0;

	sensirion_i2c_hal_init();

	clean_up_sensor_states(error);
	self_test_SGP41(error);

	start_measurement(error);
	// SGP41 Parameters for deactivated humidity compensation:
    uint16_t default_rh = 0x8000;
    uint16_t default_t = 0x6666;

	while (1) {

		// Read Measurement
		sensirion_i2c_hal_sleep_usec(1000000);	// 1 sec
			// SCD41
			bool data_ready_flag = false;
			error = scd4x_get_data_ready_flag(&data_ready_flag);
			if (error) {
				printk("Error executing scd4x_get_data_ready_flag(): %i\n", error);
				continue;
			}
			if (!data_ready_flag) {
				continue;
			}

			bool valid_SC41_data = false;
			uint16_t SC41_co2;
			int32_t SCD41_temperature;
			int32_t SCD41_humidity;
			error = scd4x_read_measurement(&SC41_co2, &SCD41_temperature, &SCD41_humidity);
			if (error) {
				printk("Error executing scd4x_read_measurement(): %i\n", error);
			} else if (SC41_co2 == 0) {
				printk("Invalid sample detected, skipping.\n");
			} else {
				valid_SC41_data = true;
			}
			// SVM41
			bool valid_SVM41_data = false;
			int16_t SVM41_humidity;
			int16_t SVM41_temperature;
			int16_t SVM41_voc_index;
			int16_t SVM41_nox_index;
			error = svm41_read_measured_values_as_integers(&SVM41_humidity, &SVM41_temperature,
														&SVM41_voc_index, &SVM41_nox_index);
			if (error) {
				printk("Error executing svm41_read_measured_values_as_integers(): "
					"%i\n",
					error);
			} else {
				valid_SVM41_data = true;
			}
			// SGP41
			bool valid_SGP41_data = false;
			uint16_t sraw_voc;
			uint16_t sraw_nox;

			error = sgp41_measure_raw_signals(default_rh, default_t, &sraw_voc,
											&sraw_nox);
			if (error) {
				printk("Error executing sgp41_measure_raw_signals(): "
					"%i\n",
					error);
			} else {
				valid_SGP41_data = true;
			}

		// Print Measurements
			if(valid_SC41_data) {
				printk("SCD41 Measurement->\n");
				printk("CO2: %u\n", SC41_co2);
				printk("Temperature: %d m°C\n", SCD41_temperature);
				printk("Humidity: %d mRH\n", SCD41_humidity);
			}

			if(valid_SVM41_data) {
				printk("SVM41 Measurement->\n");
				printk("Humidity: %i milli %% RH\n", SVM41_humidity * 10);
				printk("Temperature: %i milli °C\n", (SVM41_temperature >> 1) * 10);
				printk("VOC index: %i (index * 10)\n", SVM41_voc_index);
				printk("NOx index: %i (index * 10)\n", SVM41_nox_index);
			}

			if(valid_SGP41_data) {
				printk("SGP41 Measurement->\n");
				printk("SRAW VOC: %u\n", sraw_voc);
				printk("SRAW NOx: %u\n", sraw_nox);
			}
	}
}