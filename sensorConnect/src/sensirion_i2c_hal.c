#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/sys/printk.h>

#include "sensirion_common.h"
#include "sensirion_config.h"
#include "sensirion_i2c_hal.h"

#define I2C_NODE		DT_NODELABEL(i2c0)

LOG_MODULE_REGISTER(sensirion_i2c_hal, LOG_LEVEL_INF);

static const struct device *i2c_dev;

/**
 * Get all hard- and software components that are needed for the I2C
 * communication from Zephyr Device Tree.
 */
void sensirion_i2c_hal_get(void) {
    
    /* Get I2C Connection */
    i2c_dev = DEVICE_DT_GET(I2C_NODE);
    if (!device_is_ready(i2c_dev)) {
        
        LOG_ERR("Initialization of I2C connection failed!");
    }
    return;
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
