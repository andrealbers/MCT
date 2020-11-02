/*
 * a1.h
 *
 *  Created on: 29.10.2020
 *      Author: andre
 */

#ifndef A1_H_
#define A1_H_

//VARIABLEN
#define STOP 0
#define START 1
#define LEFT 0
#define RIGHT 1
#define LEDTIME 200

//EINGAENGE
#define TA1 6  //PO.6
#define TA2 7  //P0.7
#define TA3 8  //P0.8

//AUSGAENGE



//PROTOTYPEN
void io_init(void);
void set_led(uint32_t pos);
uint32_t getkey(void);
void delay(uint32_t dtime);
void keyhandler(uint32_t keys, uint32_t *direction, uint32_t *runstate);

#endif /* A1_H_ */
