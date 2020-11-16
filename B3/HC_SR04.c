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

#define PCRIT 1 << 16 //Power Control -> Peripherie aktivieren
#define RITEN 1 << 3  //Timer aktivieren/deaktivieren -> 1 = Aktivieren , 0 = Deaktivieren
#define PCLK_RIT 0b01 << 26 //Timer Clock auf  CCLK setzen

#define schallgeschwindigkeit 343.0 //in m/s

uint32_t getDistance(void) { //Schallgeschwindigkeit in Luft bei 20°C -> 343m/s -> 34,3cm/ms
	uint32_t duration[6] = {0,0,0,0,0,0};
	float mittelDuration = 0, usDuration = 0, distance = 0;

	LPC_SC->PCONP |= PCRIT;          //Peripherie aktivieren
	LPC_SC->PCLKSEL1 &= ~(0b11 << 26);  //Erst löschen
	LPC_SC->PCLKSEL1 |= (PCLK_RIT);       //Dann Timer Clock auf CCLK setzen
	LPC_RIT->RICTRL &= ~(RITEN); //Timer deaktivieren

	for (int i = 0; i <= 4; i++) {
		LPC_RIT->RICOUNTER = 0;
		digitalWrite(HC_Triggerpin, HCport, HIGH); //Ultraschallsensor triggern (min 10µs Signal auf HIGH halten)
		delay(500);
		digitalWrite(HC_Triggerpin, HCport, LOW); //Jetzt sendet der Ultraschallsensor einen Burst aus

		while (digitalRead(HC_Echopin, HCport) == LOW) {
			//Warten bis Signal auf High wechselt
		}

		LPC_RIT->RICTRL |= (RITEN); //Timer aktivieren

		while (digitalRead(HC_Echopin, HCport) == HIGH) {
			//Warten bis Signal wieder auf LOW wechselt -> Laufzeit = Zeit, wie lange Signal HIGH war
		}

		LPC_RIT->RICTRL &= ~(RITEN); //Timer deaktivieren

		duration[i] = LPC_RIT->RICOUNTER; //Inkrementiert in 120MHz? -> 8.3ns pro Inkrement o. 100MHz -> 10ns pro Inkrement
		duration[5] = duration[5] + duration[i];
		delay(5000);
	}
	mittelDuration = duration[5] / 5;  //Mittelwert bestimmen
	mittelDuration = mittelDuration / 2;
	usDuration = ((float) mittelDuration) * (10.0 / 1000.0); //Zeit in us die gebraucht wurde für hin bzw zurück
	distance = usDuration * ((schallgeschwindigkeit * 100.0) / 1000000.0); //Abstand in cm Hin bzw Zurück
	distance = distance * 10;  //Ausgabe in mm..

	return distance;
}

void setMatrix(void) {

}
