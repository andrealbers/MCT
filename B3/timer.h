/**
 *  \file timer.h
 */

#ifndef TIMER_H_
#define TIMER_H_

#define PCRIT 1 << 16 //Power Control -> Peripherie aktivieren
#define RITEN 1 << 3  //Timer aktivieren/deaktivieren -> 1 = Aktivieren , 0 = Deaktivieren
#define PCLK_RIT 0b01 << 26 //Timer Clock auf  CCLK setzen

void timer_init();

#endif /* TIMER_H_ */
