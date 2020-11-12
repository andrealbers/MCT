/**
 * \file a1.h
 *
 * \authors Gustav Hey, André Albers
 *
 * \date 05. November 2020
 *
 * \version 1.0
 *  Erstellung
*/

#ifndef A1_H_
#define A1_H_

//MAKROS
#define STOP 0 /**< STOP Zustand */
#define START 1 /**< START Zustand */
#define LEFT 0 /**< LED Richtung LEFT  */
#define RIGHT 1 /**< LED Richtung RIGHT */
#define LEDTIME 200 /**< Delay zwischen der Weiterschaltung der LEDs */
#define TA1 1  /**< Tasterzustand ist 1, wenn TA1 gedrückt wurde (P0.6) */
#define TA2 2   /**< Tasterzustand ist 2, wenn TA2 gedrückt wurde (P0.7) */
#define TA3 4   /**< Tasterzustand ist 4, wenn TA3 gedrückt wurde (P0.8) */


//PROTOTYPEN
void io_init(void);
void set_led(uint32_t pos);
uint32_t getkey(void);
void delay(uint32_t dtime);
void keyhandler(uint32_t keys, uint32_t *direction, uint32_t *runstate);

#endif /* A1_H_ */
