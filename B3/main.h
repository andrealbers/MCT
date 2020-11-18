/*
 * main.h
 *
 *  Created on: 17.11.2020
 *      Author: andre
 */

#ifndef MAIN_H_
#define MAIN_H_

#define I2C_PCF8574A_ADDR 0x7E


#define TA1 0b001
#define TA2 0b010
#define TA3 0b100

#define T1 0b1110
#define T2 0b1101
#define T3 0b1011
#define T4 0b0111

#define L1 0
#define L2 1
#define L3 2
#define RESET_DATA 3 //Nur zum Reset
#define L1_1 4

#define CLR_I2CLED 0b11111111
#define I2CLED1 0b11111110
#define I2CLED2 0b11111101
#define I2CLED3 0b11111011
#define I2CLED4 0b11110111



#define RGB_EIN 0
#define RGB_AUS 1

#define UNTER 0
#define UEBER 1

#define startwert 600
#define kstartwert 200
#define abtastzeit 1000

#define Leerzeile "                    "

void delay(volatile uint32_t dtime);
uint32_t geti2cKeys();

#endif /* MAIN_H_ */
