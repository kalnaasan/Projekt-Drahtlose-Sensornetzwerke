#include "sensor_functionality.h"
#include "scd4x_i2c.h"
#include "svm41_i2c.h"
#include "sensirion_common.h"
#include "sensirion_i2c_hal.h"




/* Sensor functionality */
void clean_up_sensor_states(int16_t *error)
{
	// SCD41
	scd4x_wake_up();
	scd4x_stop_periodic_measurement();
	*error = scd4x_reinit();
	if (*error)
	{
		printk("Error executing scd4x_reinit(): %i\n", *error);
		return;
	}
	// SVM41
	*error = svm41_device_reset();
	if (*error)
	{
		printk("Error executing svm41_device_reset(): %i\n", *error);
		return;
	}
}

void start_periodic_measurement(int16_t *error)
{
	// SCD41
	switch(measure_period) {
		case FIVE:		*error = scd4x_start_periodic_measurement(); break;
		case THIRTY:	*error = scd4x_start_low_power_periodic_measurement(); break;
		default:		break; //SITXTY -> single shot in idle mode 
	}
	if (*error)
	{
		printk("Error executing scd41x_[low_power_]start_periodic_measurement(): %i\n", *error);
		return;
	}
	
	// SVM41
	*error = svm41_start_measurement();
	if (*error)
	{
		printk("Error executing svm41_start_measurement(): %i\n", *error);
		return;
	}
}

void stop_periodic_measurement(int16_t *error)
{
	// SCD41
	*error = scd4x_stop_periodic_measurement();
	if (*error)
	{
		printk("Error executing scd4x_stop_periodic_measurement(): %i\n", *error);
		return;
	}
	// SVM41
	*error = svm41_stop_measurement();
	if (*error)
	{
		printk("Error executing svm41_stop_measurement(): %i\n", *error);
		return;
	}
}

void read_measurement(int16_t *error)
{
	printk("------------------------\n");

	valid_SC41_data = false;
	valid_SVM41_data = false;

	// SCD41
	bool data_ready_flag = false;

	do
	{
		*error = scd4x_get_data_ready_flag(&data_ready_flag);
		if (*error)
		{
			printk("Error executing scd4x_get_data_ready_flag(): %i\n", *error);
			continue;
		}
		if (!data_ready_flag)
		{
			printk("SCD41 Data not ready\n");
			continue;
		}
		*error = scd4x_read_measurement(&SCD41_co2, &SCD41_temperature, &SCD41_humidity);
		if (*error)
		{
			printk("Error executing scd4x_read_measurement(): %i\n", *error);
			continue;
		}
		else if (SCD41_co2 == 0)
		{
			printk("Invalid SCD41 sample detected, skipping this sensor.\n");
		}
		else
		{
			valid_SC41_data = true;
		}
		// SVM41
		*error = svm41_read_measured_values_as_integers(&SVM41_humidity, &SVM41_temperature,
													   &SVM41_voc_index, &SVM41_nox_index);
		if (*error)
		{
			printk("Error executing svm41_read_measured_values_as_integers(): %i\n", *error);
			continue;
		}
		else
		{
			valid_SVM41_data = true;
		}
	} while (false);
}

void print_measurement()
{
	printk("->Measurements\n");
	printk("SCD41\n");
	if (valid_SC41_data)
	{

		printk("\tCO2: %u\n", SCD41_co2);
		printk("\tTemperature: %d m°C\n", SCD41_temperature);
		printk("\tHumidity: %d mRH\n", SCD41_humidity);
	}
	else
		(printk("\tNo Valid Data.\n"));

	printk("SVM41\n");
	if (valid_SVM41_data)
	{
		printk("\tHumidity: %i milli %% RH\n", SVM41_humidity * 10);
		printk("\tTemperature: %i milli °C\n", (SVM41_temperature >> 1) * 10);
		printk("\tVOC index: %i (index * 10)\n", SVM41_voc_index);
		printk("\tNOx index: %i (index * 10)\n", SVM41_nox_index);
	}
	else
		(printk("\tNo Valid Data.\n"));
	printk("\n");
}

char *create_coap_message()
{
	char jsonBuffer[TEXTBUFFER_SIZE];

	int ret = snprintk(jsonBuffer, sizeof(jsonBuffer),
					   "{\"id\":\"%X%X\",\"values\":{\"scd41_co2\":%u,\"scd41_temp\":%d,\"scd41_hum\":%d,\"svm41_hum\":%i,\"svm41_temp\":%i,\"svm41_voc\":%i}}",
					   NRF_FICR->DEVICEID[0], NRF_FICR->DEVICEID[1],
					   SCD41_co2, SCD41_temperature, SCD41_humidity,
					   SVM41_humidity * 10, (SVM41_temperature >> 1) * 10, SVM41_voc_index);

	if (ret >= 0 && ret < sizeof(jsonBuffer))
	{
		printk("JSON Encoded Data: %s\n", jsonBuffer);
	}
	else
	{
		printk("JSON Encoding Failed: %d\n", ret);
	}

	char *buf = malloc(sizeof(char) * TEXTBUFFER_SIZE);
	for (int i = 0; i < TEXTBUFFER_SIZE; ++i)
	{
		buf[i] = jsonBuffer[i];
	}

	return buf;
}

char* create_coap_message_with_k_heap_alloc(struct k_heap* heap, int16_t *ret) {
	char* jsonBuffer = (char*)k_heap_alloc(heap, TEXTBUFFER_SIZE, K_NO_WAIT);

	*ret = snprintk(jsonBuffer, sizeof(jsonBuffer),
					   "{\"id\":\"%X%X\",\"values\":{\"scd41_co2\":%u,\"scd41_temp\":%d,\"scd41_hum\":%d,\"svm41_hum\":%i,\"svm41_temp\":%i,\"svm41_voc\":%i}}",
					   NRF_FICR->DEVICEID[0], NRF_FICR->DEVICEID[1],
					   SCD41_co2, SCD41_temperature, SCD41_humidity,
					   SVM41_humidity * 10, (SVM41_temperature >> 1) * 10, SVM41_voc_index);

	if (*ret >= 0 && *ret < sizeof(jsonBuffer))
	{
		printk("JSON Encoded Data: %s\n", jsonBuffer);
	}
	else
	{
		printk("JSON Encoding Failed: %d\n", *ret);
	}

	return jsonBuffer;
}

void forced_co2_recalibration(int16_t *error) {

	uint16_t target_value = CO2_TARGET_VALUE;
	uint16_t frc_correction;

	*error = scd4x_perform_forced_recalibration(target_value, &frc_correction);
	if(error) {
		printk("Error executing scd4x_perform_forced_recalibration(): %i\n", *error);
	}

	k_msleep(400);

	if(frc_correction == 0xffff) {
		*error = frc_correction;
		printk("Error: FRC Correction failed on SCD41: 0xffff\n");
		return;
	}
	printk("frc_correction: %i\n", frc_correction);
}
