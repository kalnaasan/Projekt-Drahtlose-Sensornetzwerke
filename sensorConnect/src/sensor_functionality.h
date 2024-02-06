#ifndef SENSOR_FUNCTIONALITY_H
#define SENSOR_FUNCTIONALITY_H


#include "sensirion_i2c_hal.h"

#define TEXTBUFFER_SIZE 256
#define CO2_TARGET_VALUE 400;

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

/**
 * clean_up_sensor_states() - Initializes Sensors
 * 
 * Initialize both connected Sensirion Sensors SCD41 and the Ev. Kit SVM41
 * SCD41: wake up the sensor (if sleeping) and reinitialize.
 * SVM41: trigger device reset
 * 
 * @param error	a pointer to the error return value used.
*/
void clean_up_sensor_states(int16_t* error);

/**
 * start_periodic_measurement() - Starts the periodic measurement of the Sensors
 * 
 * Depending on the measure_period of the Station, turn on the periodic measurement of the Sensors.
 * SCD41: 30 Sec or 60 Sec. For 5 Seconds the measurement is in void read_measurement()
 * SVM41: only 5Sec period possible.
 * 
 * @param error	pointer to the error return value used
 * 
 * @param svm41 true for starting per.measurement of SVM41, false otherwise
*/
void start_periodic_measurement(int16_t *error, bool svm41);

/**
 * perform_single_measurement_scd41() - Performs a single measurement shot for the SCD41
 * 
 * Is used when measure_period is 60 sec
 * 
 * @param error	pointer to the error return value used
*/
void perform_single_measurement_scd41(int16_t *error);

/**
 * stop_periodic_measurement() - Stops all measurement and sets Sensors to idle mode
 * 
 * @param error	pointer to the error return value used
*/
void stop_periodic_measurement(int16_t *error);

/**
 * read_measurement() - Reads the measurement the Sensors have taken
 * 
 * @param error	pointer to the error return value used
*/
void read_measurement(int16_t *error);

/**
 * print_measurement_to_console() - Prints the read values onto the Console
*/
void print_measurement_to_console();

/**
 * create_coap_message() - Creates a message for CoAP Communication with the CoAP Server.
 * 
 * Creates a CoAP message by parsing the read Sensor values into a JSON format, that the CoAP Server can read.
*/
char *create_coap_message();

/**
 * forced_co2_recalibration() - Performs a forced recalibration (FRC) on the SCD41 Sensor
 * 
 * Performs a FRC (forced recalibration) on the Sensirion Sensor SCD41 for the Co2 measurement.
 * If the correction of the offset for Co2 on the SCD41 is 0xffff, the FRC failed and the 
 * function exits with this as the Error.
 * 
 * @param error	pointer to the error return value used
 * 
 * @returns a string with Sensor values in JSON format
*/
void forced_co2_recalibration(int16_t *error);

/**
 * measure_period_to_string() - Returns the String of measure period
 * 
 * @param m_period	the targeted enum
 * 
 * @returns a string representation of the enum
*/
static const char* measure_period_to_string(enum measure_period m_period) {
	switch (m_period) {	
        case FIVE:
            return "FIVE";
        case THIRTY:
            return "THIRTY";
        case SIXTY:
            return "SIXTY";
        default:
            return "Unknown";
    }
}

/**
 * calib_mode_to_string() - Returns the String of calibration mode
 * 
 * @param m_period	the targeted enum
 * 
 * @returns a string representation of the enum
*/
static const char* calib_mode_to_string(enum calibration_mode c_mode) {
	switch (c_mode) {	
        case CO2:
            return "CO2";
        case TEMP:
            return "TEMP";
        case VOC:
            return "VOC";
        default:
            return "Unknown";
    }
}

#endif /* SENSOR_FUNCTIONALITY_H */