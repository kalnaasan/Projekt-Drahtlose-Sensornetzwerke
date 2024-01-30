#ifndef SENSOR_FUNCTIONALITY_H
#define SENSOR_FUNCTIONALITY_H


#include "sensirion_config.h"

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


static bool valid_SC41_data = false;
static bool valid_SVM41_data = false;

void clean_up_sensor_states(int16_t* error);

void start_measurement(int16_t* error);

void read_measurement();

void print_measurement();

#endif /* SENSOR_FUNCTIONALITY_H */