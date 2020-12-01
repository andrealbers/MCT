/**
 *  \file interrupts.c
 */

#include "interrupts.h"

uint32_t changed = 0; //Wird im EINT3_IRQHandler() gesetzt, wenn ein Taster von T1-T4 gedrückt wurde

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
	LPC_SC->PCONP |= PCRIT;             //Peripherie aktivieren
	LPC_SC->PCLKSEL1 &= ~(0b11 << 26);  //Erst löschen
	LPC_SC->PCLKSEL1 |= (PCLK_RIT);     //Dann Timer Clock auf CCLK setzen
	LPC_RIT->RICTRL &= ~(RITEN);        //Timer deaktivieren
	LPC_RIT->RICOUNTER = 0;             //Zähler zurücksetzen
}

/**
 * \brief <b>Konfiguration des externen Interrupts 3</b><br>
 * Hier erfolgt die Initialisierung des externen Interrupts 3 (EINT3)<br>
 * - Funktion des angeschlossenen Pins im Pin function select register 4 als Externen Interrupt einstellen
 * - Im Pin Mode select register 4 den Pin in ein Pullup-Netzwerk schalten
 * - Setzen des 3. Bits im External Interrupt Mode register, zur Konfiguration als flankengesteuerten Interrupt
 * - Löschen des 3. Bits im External Interrupt Polarity register, um Interrupt bei fallender Flanke auszulösen
 * - Setzen des 21. Bits im Interrupt Set-Enable Register 0, um Interrupt zu aktivieren
 */
void EINT3_init() {
	LPC_PINCON->PINSEL4 &= ~(0b11 << (EINT3_pin * 2));   //Bits löschen
	LPC_PINCON->PINSEL4 |= (0b01 << (EINT3_pin * 2));    //Funktion als EINT3 konfigurieren
	LPC_PINCON->PINMODE4 &= ~(0b11 << (EINT3_pin * 2));  //Pullup-Widerstand zuschalten
	LPC_SC->EXTMODE |= (1 << 3);    //Als flankengesteuerten Interrupt setzen
	LPC_SC->EXTPOLAR &= ~(1 << 3);  //Bei fallender Flanke auslösen
	NVIC_EnableIRQ(EINT3_IRQn);     //Interrupt im Nested Vector Interrupt Controller freigeben
}

/**
* \brief <b>External Interrupt 3 Handler</b><br>
* - Wird aufgerufen, wenn ein Taster T1-T4 gedrückt wurde. <br>
* - Setzen der globalen Variable changed auf 1, um in der Funktion getTkeys() die i2c-Verbindung zum Portexpander aufzubauen.
* */
void EINT3_IRQHandler() {
	LPC_SC->EXTINT = (1 << 3);  //Löschen des Pending Bits nach ausgelöstem EINT3
	changed = 1;
}
