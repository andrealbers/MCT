/**
 *  \file PCF8574.h
 */

#ifndef PCF8574_H_
#define PCF8574_H_

#include "main.h"

//MAKROS
#define I2C_PCF8574A_ADDR 0x7E

#define T1 0b1110  //T1 gedrückt
#define T2 0b1101  //T2 gedrückt
#define T3 0b1011  //T3 gedrückt
#define T4 0b0111  //T4 gedrückt
#define Tnokey 0b1111 //Zustand, wenn kein T-Taster gedrückt wurde

#define CLR_I2CLED 0b11111111  //Zum Ausschalten der LEDs LED1-LED4
#define I2CLED1 0b11111110     //Einschalten von LED1
#define I2CLED2 0b11111101     //Einschalten von LED2
#define I2CLED3 0b11111011     //Einschalten von LED3
#define I2CLED4 0b11110111     //Einschalten von LED4

//PROTOYPEN
void writeFrontLED(uint32_t led);
uint32_t getTkeys();
#endif /* PCF8574_H_ */
