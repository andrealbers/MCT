/**
 *  \file GPIO.h
 */

#ifndef GPIO_H_
#define GPIO_H_

static LPC_GPIO_TypeDef (*const LPC_GPIO[5]) = { LPC_GPIO0, LPC_GPIO1, LPC_GPIO2, LPC_GPIO3, LPC_GPIO4 }; //Um LPC_GPIO[port] zu verwenden

//MAKROS FÜR pinMode (Bitkombination um PINMODE zu setzen)
#define PULLUP 0b00
#define OUTPUT 0b10
#define PULLDOWN 0b11

//Allgemeine Makros
#define LOW 0
#define HIGH 1

//Taster links auf Frontplatte
#define TAport 0
#define TA1pin 6
#define TA2pin 7
#define TA3pin 8

//Ultraschallsensor
#define HC_Triggerpin 0   //Triggersignal für Ultraschallsensor
#define HC_Echopin 1      //Echosignal vom Ultraschallsensor
#define HCport 0

//4 Stelliges 7segment Display mit TM1637
#define Siebensegment_CLKpin 10
#define Siebensegment_DIOpin 11
#define Siebensegmentport 0

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

//RGB-LED auf Mainboard
#define RGB_Rpin 22    //ROT
#define RGB_Rport 0
#define RGB_Gpin 25    //GRÜN
#define RGB_Bpin 26    //BLAU
#define RGB_GBport 3
#define RGB_R 0b001    //Bitkodierung für die rote RGB LED
#define RGB_G 0b010    //Bitkodierung für die grüne RGB LED
#define RGB_B 0b100    //Bitkodierung für die blaue RGB LED
#define RGB_AUS 0      //Zustand für RGB ausschalten

//Lautsprecher
#define Speakerpin 9
#define Speakerport 0


//PROTOTYPEN
void pinMode(uint32_t pin, uint32_t portnr, uint32_t mode);
void digitalWrite(uint32_t pin, uint32_t port, uint32_t set);
uint32_t digitalRead(uint32_t pin, uint32_t port);
void set_rgb(uint32_t rgb_led);

#endif /* GPIO_H_ */
