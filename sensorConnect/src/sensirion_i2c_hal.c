//#include <nrf_delay.h>
//#include <nrf_drv_twi.h>
//#include <stdio.h>

#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/sys/printk.h>

#include "sensirion_common.h"
#include "sensirion_config.h"
#include "sensirion_i2c_hal.h"

/**
 * Nordic specific configuration. Not needed as they are within the i2c
 * device. ----->
 */
//#define SENSIRION_SDA_PIN 0
//#define SENSIRION_SCL_PIN 2

/**
 * New TWI instance.
 */
//static const nrf_drv_twi_t i2c_instance = NRF_DRV_TWI_INSTANCE(0);
#define I2C_NODE		DT_NODELABEL(i2c0)
static const struct device *i2c_dev;

/**
 * Initialize all hard- and software components that are needed for the I2C
 * communication.
 */
void sensirion_i2c_hal_init(void) {
    
    /* initiate TWI instance */
    i2c_dev = DEVICE_DT_GET(I2C_NODE);
    if (!device_is_ready(i2c_dev)) {
        
        printk("Initialization of I2C connection failed!\n");
    }
    /* enable TWI instance */
    // -----> is this needed with the device here?
    return;
}

/**
 * Release all resources initialized by sensirion_i2c_hal_init().
 */
void sensirion_i2c_hal_free(void) {
    // -----> Check, if needed.
}

/**
 * Execute one read transaction on the I2C bus, reading a given number of bytes.
 * If the device does not acknowledge the read command, an error shall be
 * returned.
 *
 * @param address 7-bit I2C address to read from
 * @param data    pointer to the buffer where the data is to be stored
 * @param count   number of bytes to read from I2C and store in the buffer
 * @returns 0 on success, error code otherwise
 *
 * error codes:  3 -> error detected by hardware (internal error)
 *              17 -> driver not ready for new transfer (busy)
 */
int8_t sensirion_i2c_hal_read(uint8_t address, uint8_t* data, uint16_t count) {
    int8_t err = i2c_read(i2c_dev, data, (uint8_t)count, address);
    return err;
}

/**
 * Execute one write transaction on the I2C bus, sending a given number of
 * bytes. The bytes in the supplied buffer must be sent to the given address. If
 * the slave device does not acknowledge any of the bytes, an error shall be
 * returned.
 *
 * @param address 7-bit I2C address to write to
 * @param data    pointer to the buffer containing the data to write
 * @param count   number of bytes to read from the buffer and send over I2C
 * @returns 0 on success, error code otherwise
 *
 * error codes:  3 -> error detected by hardware (internal error)
 *              17 -> driver not ready for new transfer (busy)
 */
int8_t sensirion_i2c_hal_write(uint8_t address, const uint8_t* data,
                               uint16_t count) {
    int8_t err = i2c_write(i2c_dev, data, (uint8_t)count, address);
    return err;
}

/**
 * Sleep for a given number of microseconds. The function should delay the
 * execution for at least the given time, but may also sleep longer.
 *
 * @param useconds the sleep time in microseconds
 */
void sensirion_i2c_hal_sleep_usec(uint32_t useconds) {
    k_usleep(useconds);
}
