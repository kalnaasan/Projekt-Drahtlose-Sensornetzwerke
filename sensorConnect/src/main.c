#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/sys/printk.h>

#define SLEEP_TIME_MS 2000

#define SCD41_I2C_ADDRESS	98
#define SCD41_I2C_START_PERIODIC_MEASUREMENT	0x21B1


#define I2C_NODE		DT_NODELABEL(i2c0)
static const struct device *i2c_dev = DEVICE_DT_GET(I2C_NODE);

static uint8_t i2c_buffer[9];
static uint16_t offset;

uint16_t sensirion_i2c_add_command_to_bufferMy(uint8_t* buffer, uint16_t offset,
                                             uint16_t command) {
    buffer[offset++] = (uint8_t)((command & 0xFF00) >> 8);
    buffer[offset++] = (uint8_t)((command & 0x00FF) >> 0);
    return offset;
}
void clear_buffer(uint8_t* buffer) {
	for(int i = 0; i < 9; i++) buffer[i] = 0;
}

void printErr(int err, char* errType, uint8_t* buffer) {
	printk("ERR = %d |\t Type: %s, Buffer = [%d, %d] -> HEX %#2x%2x\n", err, errType, buffer[0], buffer[1], buffer[0], buffer[1]);
}
void printSuc(char* errType, uint8_t* buffer) {
	printk("Command-> %s |\t Buffer = [%d, %d] -> HEX %#2x%2x\n", errType, buffer[0], buffer[1], buffer[0], buffer[1]);
}

void main(void){

	int8_t err = 0;

	// Initialize I2C

	if(!device_is_ready(i2c_dev)) { 
		printk("i2c_dev not ready\n");
		return;
	} else {
		printk("i2c_dev initialized\n");
	}

	//wake up Sensor

	offset = 0;
	clear_buffer(i2c_buffer);
	offset = sensirion_i2c_add_command_to_bufferMy(&i2c_buffer[0], offset, 0x36F6);
	err = i2c_write(i2c_dev, i2c_buffer, offset, SCD41_I2C_ADDRESS);
	if( err < 0) {printErr(err, "W: Wake Up", &i2c_buffer[0]);}
	else {printSuc("W: Wake Up", &i2c_buffer[0]);}
	k_msleep(SLEEP_TIME_MS*2);

	// clean up states before 
		//stop per. measurement
	offset = 0;
	clear_buffer(i2c_buffer);
	offset = sensirion_i2c_add_command_to_bufferMy(&i2c_buffer[0], offset, 0x3F86);
	err = i2c_write(i2c_dev, i2c_buffer, offset, SCD41_I2C_ADDRESS);
	if( err < 0) {printErr(err, "W: Stop Meas.", &i2c_buffer[0]);}
	else {printSuc("W: Stop Meas.", &i2c_buffer[0]);}
	
	k_msleep(SLEEP_TIME_MS*5);
    	// reinit
	offset = 0;
	clear_buffer(i2c_buffer);
	offset = sensirion_i2c_add_command_to_bufferMy(&i2c_buffer[0], offset, 0x3646);
	err = i2c_write(i2c_dev, i2c_buffer, offset, SCD41_I2C_ADDRESS);
	if( err < 0) {printErr(err, "W: ReInit", &i2c_buffer[0]);}
	else {printSuc("W: ReInit", &i2c_buffer[0]);}
	k_msleep(SLEEP_TIME_MS*5);

	// Start Measurement
	do {
		offset = 0;
		clear_buffer(i2c_buffer);
		offset = sensirion_i2c_add_command_to_bufferMy(&i2c_buffer[0], offset, SCD41_I2C_START_PERIODIC_MEASUREMENT);
		err = i2c_write(i2c_dev, i2c_buffer, offset, SCD41_I2C_ADDRESS);
		if( err < 0) {printErr(err, "W: Start Meas.", &i2c_buffer[0]);}
		else {printSuc("W: Start Meas.", &i2c_buffer[0]);}
	} while(false);
	
	printk("Waiting for first measurement... (5 sec)\n");
	k_msleep(5000);


	while (1) {
		
		// Read Measurement
		do{
			printk("-----NEW READ-----\n");
			// Check if data is ready
			/*
			offset = 0;
			clear_buffer(i2c_buffer);
			offset = sensirion_i2c_add_command_to_bufferMy(&i2c_buffer[0], offset, 0xE4B8);
			err = i2c_write(i2c_dev, i2c_buffer, 2, SCD41_I2C_ADDRESS);
			if( err < 0) {printErr(err, "DataCheck - write", &i2c_buffer[0]); break;}
			else {printSuc(err, "DataCheck - write", &i2c_buffer[0]);}

			k_msleep(1000);

			clear_buffer(i2c_buffer);
			err = i2c_read(i2c_dev, i2c_buffer, 2, SCD41_I2C_ADDRESS);
			if( err < 0) {printErr(err, "DataCheck - read", &i2c_buffer[0]); break;}
			else {printSuc(err, "DataCheck - read", &i2c_buffer[0]);}

			
			int data_ready = (uint16_t)i2c_buffer[0] << 8 | (uint16_t)i2c_buffer[1];
			if ((data_ready & 0x07FF) != 0) {printk("(%#4x & 0x07FF) != 0", data_ready);continue;}
			*/
			// Read Data
			uint16_t co2;
        	int32_t temperature;
        	int32_t humidity;

			offset = 0;
			clear_buffer(i2c_buffer);
			offset = sensirion_i2c_add_command_to_bufferMy(&i2c_buffer[0], offset, 0xEC05);
			err = i2c_write(i2c_dev, i2c_buffer, offset, SCD41_I2C_ADDRESS); 
			if( err < 0) {printErr(err, "W: GetData", &i2c_buffer[0]); break;}
			else {printSuc("W: GetData", &i2c_buffer[0]);}

			k_msleep(1000);

			
			err = i2c_read(i2c_dev, i2c_buffer, 6, SCD41_I2C_ADDRESS);
			if( err < 0) {printErr(err, "W: GetData", &i2c_buffer[0]); break;}
			else {printSuc("W: GetData", &i2c_buffer[0]);}

			// Calculate Measurements
			co2 = (uint16_t)i2c_buffer[0] << 8 | (uint16_t)i2c_buffer[1];
    		temperature = (uint16_t)i2c_buffer[2] << 8 | (uint16_t)i2c_buffer[3];
    		humidity = (uint16_t)i2c_buffer[4] << 8 | (uint16_t)i2c_buffer[5];

			// Print Data
			if (co2 == 0) {
				printk("Invalid sample detected, skipping.\n");
			} else {
				printk("CO2: %u\n", co2);
				printk("Temperature: %d m°C\n", temperature);
				printk("Humidity: %d mRH\n", humidity);
			}

		}while(false);

		k_msleep(SLEEP_TIME_MS*5);
	}
}