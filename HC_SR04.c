/*
 * HC_SR04.c
 *
 *  Created on: 16.11.2020
 *      Author: andre
 */
#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif

#include "i2c_1769.h"
#include "GPIO.h"

#define PCRIT 1 << 16   //RIT aktivieren
#define PCLK_RIT 0b10 << 26   //Frequenz für Inkrementierung des Counters -> PCLK_peripherial = CCLK
#define RITENCLR 0 << 1
#define RITEN 1 << 3

uint32_t getDistance(void) {
	uint32_t time = 0, distance = 0;
	LPC_SC->PCONP |= PCRIT;
	LPC_SC->PCLKSEL1 &= ~(0b11 << 26);
	LPC_SC->PCLKSEL1 |= PCLK_RIT;
	LPC_RIT->RICTRL &= ~(RITENCLR);
	LPC_RIT->RICTRL |= (RITEN);  //Timer aktivieren



	digitalWrite(HC_Triggerpin, HCport, HIGH); //Ultraschallsensor triggern (min 10µs Signal auf HIGH halten)
	digitalWrite(LED0pin, yLEDport, LOW);
	delay(100);
	digitalWrite(HC_Triggerpin, HCport, LOW);
	digitalWrite(LED0pin, yLEDport, HIGH);

	while (digitalRead(HC_Echopin, HCport) != 1) {   //Warten bis Signal auf High wechselt
	}

	while (digitalRead(HC_Echopin, HCport)) {      //So lange Signal High ist -> Zeit erhöhen
		time++;
	}


	return LPC_RIT->RICOUNTER;
}

void setMatrix(void) {

}
