/*
 * PCA_9539.h
 *
 *  Created on: 18.11.2020
 *      Author: andre
 */

#ifndef PCA_9539_H_
#define PCA_9539_H_

#define I2C_PCA9539_ADDR 0xE8

#define i2cRoteLEDsAN 0x00
#define i2cRoteLEDsAUS 0xFF


void PCA9539_init();
void writei2cRedLED(uint32_t setleds);

#endif /* PCA_9539_H_ */
