/* Optional OT Features*/
#if defined(CONFIG_BT)
#include "ble.h"
#endif

#if defined(CONFIG_CLI_SAMPLE_LOW_POWER)
#include "low_power.h"
#endif

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/usb/usb_device.h>
#include <zephyr/sys/printk.h>

#include "scd4x_i2c.h"
#include "svm41_i2c.h"
#include "sensirion_common.h"
#include "sensirion_i2c_hal.h"

/* OT CLI Configuration */
LOG_MODULE_REGISTER(cli_sample, CONFIG_OT_COMMAND_LINE_INTERFACE_LOG_LEVEL);

#define WELLCOME_TEXT \
	"\n\r"\
	"\n\r"\
	"OpenThread Command Line Interface is now running.\n\r" \
	"Use the 'ot' keyword to invoke OpenThread commands e.g. " \
	"'ot thread start.'\n\r" \
	"For the full commands list refer to the OpenThread CLI " \
	"documentation at:\n\r" \
	"https://github.com/openthread/openthread/blob/master/src/cli/README.md\n\r"


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
	scd4x_reinit();
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


void ot_cli_thread(void)
{
	printk("ot_cli_thread started\n");

	#if DT_NODE_HAS_COMPAT(DT_CHOSEN(zephyr_shell_uart), zephyr_cdc_acm_uart)
		int ret;
		const struct device *dev;
		uint32_t dtr = 0U;

		ret = usb_enable(NULL);
		if (ret != 0) {
			LOG_ERR("Failed to enable USB");
			return 0;
		}

		dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_shell_uart));
		if (dev == NULL) {
			LOG_ERR("Failed to find specific UART device");
			return 0;
		}

		LOG_INF("Waiting for host to be ready to communicate");

		/* Data Terminal Ready - check if host is ready to communicate */
		while (!dtr) {
			ret = uart_line_ctrl_get(dev, UART_LINE_CTRL_DTR, &dtr);
			if (ret) {
				LOG_ERR("Failed to get Data Terminal Ready line state: %d",
					ret);
				continue;
			}
			k_msleep(100);
		}

		/* Data Carrier Detect Modem - mark connection as established */
		(void)uart_line_ctrl_set(dev, UART_LINE_CTRL_DCD, 1);
		/* Data Set Ready - the NCP SoC is ready to communicate */
		(void)uart_line_ctrl_set(dev, UART_LINE_CTRL_DSR, 1);
	#endif

		LOG_INF(WELLCOME_TEXT);

	#if defined(CONFIG_BT)
		ble_enable();
	#endif

	#if defined(CONFIG_CLI_SAMPLE_LOW_POWER)
		low_power_enable();
	#endif
}

void sensor_reading_thread(void)
{
	printk("sensor_reading_thread started\n");
	int16_t error = 0;

	sensirion_i2c_hal_init();

	clean_up_sensor_states(error);

	start_measurement(error);

	while (1) {
		printk("->\t New Reading of Sensor Node\n");
		// Read Measurement
		sensirion_i2c_hal_sleep_usec(1000000*2);	// 1 sec
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

			bool valid_SC41_data = false;
			uint16_t SC41_co2;
			int32_t SCD41_temperature;
			int32_t SCD41_humidity;
			error = scd4x_read_measurement(&SC41_co2, &SCD41_temperature, &SCD41_humidity);
			if (error) {
				printk("Error executing scd4x_read_measurement(): %i\n", error);
			} else if (SC41_co2 == 0) {
				printk("Invalid SCD41 sample detected, skipping this sensor.\n");
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

		// Print Measurements
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

// Define and initialize threads
K_THREAD_DEFINE(thread0_id, THREAD0_STACKSIZE, ot_cli_thread, NULL, NULL, NULL,
		THREAD0_PRIORITY, 0, 5000);

K_THREAD_DEFINE(thread1_id, THREAD1_STACKSIZE, sensor_reading_thread, NULL, NULL, NULL,
		THREAD1_PRIORITY, 0, 5000);