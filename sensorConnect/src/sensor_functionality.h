#ifndef SENSOR_FUNCTIONALITY_H
#define SENSOR_FUNCTIONALITY_H


#include "sensirion_i2c_hal.h"

/* FOR STATE MACHINES

typedef enum{
    SLEEP,
	SENSOR_INIT,
	PERIODIC_MEASURING,   
} sensor_state;

static sensor_state s_state = SLEEP;

*/
/* for now, data is stored like this, will be omitted later*/
static uint16_t SCD41_co2;
static int32_t SCD41_temperature;
static int32_t SCD41_humidity;

static int16_t SVM41_humidity;
static int16_t SVM41_temperature;
static int16_t SVM41_voc_index;
static int16_t SVM41_nox_index;


static enum measure_period { FIVE, THIRTY, SIXTY} measure_period;
static enum calibration_mode { CO2, TEMP, VOC} calib_mode;
static bool valid_SC41_data = false;
static bool valid_SVM41_data = false;

void clean_up_sensor_states(int16_t* error);

void start_periodic_measurement(int16_t* error);

void stop_periodic_measurement(int16_t *error);

void read_measurement();

void print_measurement();

char* create_coap_message(struct k_heap* heap, int16_t *ret);

void forced_co2_recalibration(int16_t *error);

#endif /* SENSOR_FUNCTIONALITY_H */