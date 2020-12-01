/**
 *  \file interrupts.h
 */

#ifndef INTERRUPTS_H_
#define INTERRUPTS_H_

#include "main.h"

//Globale Variable
extern uint32_t changed;

//MAKROS
#define PCRIT 1 << 16 //Power Control -> Peripherie aktivieren
#define RITEN 1 << 3  //Timer aktivieren/deaktivieren -> 1 = Aktivieren , 0 = Deaktivieren
#define PCLK_RIT 0b01 << 26 //Timer Clock auf  CCLK setzen

#define EINT3_pin 13
#define EINT3_port 2

//PROTOTYPEN
void timer_init();
void EINT3_init();

#endif /* INTERRUPTS_H_ */
