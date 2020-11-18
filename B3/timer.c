/*
 * timer.c
 *
 *  Created on: 18.11.2020
 *      Author: andre
 */
#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif

#include "timer.h"

void timer_init() {
	LPC_SC->PCONP |= PCRIT;          //Peripherie aktivieren
	LPC_SC->PCLKSEL1 &= ~(0b11 << 26);  //Erst löschen
	LPC_SC->PCLKSEL1 |= (PCLK_RIT);       //Dann Timer Clock auf CCLK setzen
	LPC_RIT->RICTRL &= ~(RITEN); //Timer deaktivieren
	LPC_RIT->RICOUNTER = 0;  //Zähler zurücksetzen

}
