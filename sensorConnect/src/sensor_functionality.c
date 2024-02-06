#include "sensor_functionality.h"
#include "scd4x_i2c.h"
#include "svm41_i2c.h"
#include "sensirion_common.h"
#include "sensirion_i2c_hal.h"


LOG_MODULE_REGISTER(sensor_functionality, LOG_LEVEL_INF);

void clean_up_sensor_states(int16_t *error)
{
	// SCD41
	scd4x_wake_up();
	scd4x_stop_periodic_measurement();
	*error = scd4x_reinit();
	if (*error)
	{
		LOG_ERR("Error executing scd4x_reinit(): %i", *error);
		return;
	}
	// SVM41
	*error = svm41_device_reset();
	if (*error)
	{
		LOG_ERR("Error executing svm41_device_reset(): %i", *error);
		return;
	}
}

void start_periodic_measurement(int16_t *error, bool svm41)
{
	*error = 0;
	
	switch(measure_period) {

		case FIVE:		*error = scd4x_start_periodic_measurement(); break;
		case THIRTY:	*error = scd4x_start_low_power_periodic_measurement(); break;
		case SIXTY:		*error = scd4x_stop_periodic_measurement();break; //SITXTY -> single shot in idle mode, measure single shot before reading
		
		default:		LOG_ERR("Error measure_period unkown: %i", measure_period);
						return;
	}

	if (*error)
	{
		LOG_ERR("Error executing scd4x_[low_power_]start_periodic_measurement(): %i", *error);
		return;
	}
	
		
	
	if(svm41) {
		*error = svm41_start_measurement();
		if (*error)
		{
			LOG_ERR("Error executing svm41_start_measurement(): %i", *error);
			return;
		}
	}
}

void perform_single_measurement_scd41(int16_t *error) {
	*error = 0;

	*error = scd4x_measure_single_shot();
	if (*error)
	{
		LOG_ERR("Error executing scd4x_measure_single_shot(): %i", *error);
	}
}

void stop_periodic_measurement(int16_t *error)
{
	// SCD41
	*error = scd4x_stop_periodic_measurement();
	if (*error)
	{
		LOG_ERR("Error executing scd4x_stop_periodic_measurement(): %i", *error);
		return;
	}
	// SVM41
	*error = svm41_stop_measurement();
	if (*error)
	{
		LOG_ERR("Error executing svm41_stop_measurement(): %i", *error);
		return;
	}
}

void read_measurement(int16_t *error)
{
	valid_SC41_data = false;
	valid_SVM41_data = false;

	// SCD41
	bool data_ready_flag = false;

	do
	{
		*error = scd4x_get_data_ready_flag(&data_ready_flag);
		if (*error)
		{
			LOG_ERR("Error executing scd4x_get_data_ready_flag(): %i", *error);
			continue;
		}
		if (!data_ready_flag)
		{
			LOG_INF("SCD41 Data not ready\n");
			continue;
		}
		*error = scd4x_read_measurement(&SCD41_co2, &SCD41_temperature, &SCD41_humidity);
		if (*error)
		{
			LOG_ERR("Error executing scd4x_read_measurement(): %i", *error);
			continue;
		}
		else if (SCD41_co2 == 0)
		{
			LOG_INF("Invalid SCD41 sample detected, skipping this sensor.");
		}
		else
		{
			valid_SC41_data = true;
			LOG_INF("SCD41 values > CO2: %u (ppm), T: %d (mC), RH: %d (mRH)", SCD41_co2, SCD41_temperature, SCD41_humidity);
		}
		// SVM41
		*error = svm41_read_measured_values_as_integers(&SVM41_humidity, &SVM41_temperature,
													   &SVM41_voc_index, &SVM41_nox_index);
		if (*error)
		{
			LOG_ERR("Error executing svm41_read_measured_values_as_integers(): %i", *error);
			continue;
		}
		else
		{
			valid_SVM41_data = true;
			LOG_INF("SVM41 values > RH: %i (mRH), t: %i (mC), VOC-idx: %i (index * 10), NOx_idx: %i )index * 10)", 
					SVM41_humidity * 10, (SVM41_temperature >> 1) * 10, SVM41_voc_index, SVM41_nox_index);
		}
	} while (false);
}

void print_measurement_to_console()
{
	printk("->Measurements\n");
	printk("SCD41\n");
	if (valid_SC41_data)
	{

		printk("\tCO2: %uppm\n", SCD41_co2);
		printk("\tTemperature: %d mC\n", SCD41_temperature);
		printk("\tHumidity: %d mRH\n", SCD41_humidity);
	}
	else
		(printk("\tNo Valid SCD41 Data.\n"));

	printk("SVM41\n");
	if (valid_SVM41_data)
	{
		printk("\tHumidity: %i mRH\n", SVM41_humidity * 10);
		printk("\tTemperature: %i mC\n", (SVM41_temperature >> 1) * 10);
		printk("\tVOC index: %i (index * 10)\n", SVM41_voc_index);
		printk("\tNOx index: %i (index * 10)\n", SVM41_nox_index);
	}
	else
		(printk("\tNo Valid SVM41 Data.\n"));
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
		LOG_ERR("JSON Encoded Data: %s", jsonBuffer);
	}
	else
	{
		LOG_INF("JSON Encoding Failed: %d", ret);
	}

	char *buf = malloc(sizeof(char) * TEXTBUFFER_SIZE);
	for (int i = 0; i < TEXTBUFFER_SIZE; ++i)
	{
		buf[i] = jsonBuffer[i];
	}

	return buf;
}

void forced_co2_recalibration(int16_t *error) {

	uint16_t target_value = CO2_TARGET_VALUE;
	uint16_t frc_correction;

	*error = scd4x_perform_forced_recalibration(target_value, &frc_correction);
	if(*error) {
		LOG_ERR("Error executing scd4x_perform_forced_recalibration(): %i", *error);
	}

	if(frc_correction == 0xffff) {
		*error = frc_correction;
		LOG_ERR("Error: FRC Correction failed on SCD41: 0xffff");
		return;
	}
	LOG_INF("frc_correction: %i", frc_correction);
}
