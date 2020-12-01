/**
 *  \file timer.c
 */

#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif

#include "timer.h"

/**
 * \brief <b>RIT initialisieren</b><br>
 * - Im Power Control for Peripherals Register (PCONP) wird das power/clock control bit gesetzt, um überhaupt die Peripherie einzuschalten.
 * Das Bit für den RIT ist 16
 * - Im Peripheral Clock Selection register 1 (PCKSEL1) wird die Taktfrequenz für den RIT gesetzt.
 *   Dieser wird über das Makro PCLK_RIT auf CCLK gesetzt (100MHz)
 * - Timer wird über das Repetitive Interrupt register deaktiviert
 * - Zählerstand von RIT (RICOUNTER) wird auf 0 gesetzt
 */
void timer_init() {
	LPC_SC->PCONP |= PCRIT;          //Peripherie aktivieren
	LPC_SC->PCLKSEL1 &= ~(0b11 << 26);  //Erst löschen
	LPC_SC->PCLKSEL1 |= (PCLK_RIT);       //Dann Timer Clock auf CCLK setzen
	LPC_RIT->RICTRL &= ~(RITEN); //Timer deaktivieren
	LPC_RIT->RICOUNTER = 0;  //Zähler zurücksetzen
}
