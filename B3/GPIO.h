/*
 * GPIO.h
 *
 *  Created on: 13.11.2020
 *      Author: andre
 */

#ifndef GPIO_H_
#define GPIO_H_

static LPC_GPIO_TypeDef (* const LPC_GPIO[5]) = { LPC_GPIO0, LPC_GPIO1, LPC_GPIO2, LPC_GPIO3, LPC_GPIO4  };
/*
 struct GPIO {
 unsigned int port;
 unsigned int bit;
 unsigned int offset;
 };
 */
//MAKROS FÜR io_init
#define PULLUP 0b00
#define OUTPUT 0b10
#define PULLDOWN 0b11

//MAKROS FÜR GPIO
#define LOW 0
#define HIGH 1

//Taster links auf Frontplatte
#define TAport 0
#define TA1pin 6
#define TA2pin 7
#define TA3pin 8

//Ultraschallsensor
#define HC_Triggerpin 0   //Triggersignal für Ultraschallsensor
#define HC_Echopin 1   //Echosignal vom Ultraschallsensor
#define HCport 0

//Orange LEDs auf Mainboard
#define yLEDport 2
#define LED0pin 1
#define LED1pin 2
#define LED2pin 3
#define LED3pin 4
#define LED4pin 5
#define LED5pin 6
#define LED6pin 7
#define LED7pin 8



//PROTOTYPEN
void digitalWrite(uint32_t pin, uint32_t port, uint32_t set);
uint32_t digitalRead(uint32_t pin, uint32_t port);
void pinMode(unsigned int pin, unsigned int portnr, unsigned int mode);
#endif /* GPIO_H_ */