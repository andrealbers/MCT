/*
 * PCA9539.c
 *
 *  Created on: 18.11.2020
 *      Author: andre
 */
#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif
#include "PCA_9539.h"
#include "i2c_1769.h"

void writei2cRedLED(uint32_t setleds) {
	if (setleds == i2cRoteLEDsAUS) {
		i2c_start();
		i2c_write_byte(I2C_PCA9539_ADDR | I2C_WRITE);
		i2c_write_byte(0x06);
		i2c_write_byte(0xff); //Pins werden als Eingang gesetzt, da die LEDs bei Aus noch leicht leuchten!
		i2c_write_byte(0xff);
	} else {
		//LEDs schalten (alle 16)
		i2c_start();
		i2c_write_byte(I2C_PCA9539_ADDR | I2C_WRITE);
		i2c_write_byte(0x07);
		i2c_write_byte(0x00);
		i2c_stop();

		i2c_start();
		i2c_write_byte(I2C_PCA9539_ADDR | I2C_WRITE);
		i2c_write_byte(0x03);
		i2c_write_byte(0x00);
		i2c_stop();
	}

}
