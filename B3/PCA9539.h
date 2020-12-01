/**
 *  \file PCA9539.h
 */

#ifndef PCA539_H_
#define PCA9539_H_

#include "main.h"

//MAKROS
#define I2C_PCA9539_ADDR 0xE8

#define i2cRoteLEDsAN 0x00
#define i2cRoteLEDsAUS 0xFF


//PROTOTYPEN
void PCA9539_init();
void writei2cRedLED(uint32_t setleds);

#endif /* PCA9539_H_ */
