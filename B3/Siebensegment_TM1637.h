/*
 * Siebensegment_TM1637.h
 *
 *  Created on: 17.11.2020
 *      Author: andre
 */

#ifndef SIEBENSEGMENT_TM1637_H_
#define SIEBENSEGMENT_TM1637_H_

#define wrDataToReg 0b01000000

#define DisplayOFF 0b10001000 //Display AUS
#define DisplayON 0b10001000 //Display AN

#define bitdelay 5 //Delay zwischen Umschalten Clk

uint32_t segmente[4] = { 0b11000000,   //1
		0b11000001,   //2
		0b11000010,   //3
		0b11000011,   //4
		};

uint32_t zahlen[10] = { 0b00111111,    // 0
		0b00000110,    // 1
		0b01011011,    // 2
		0b01001111,    // 3
		0b01100110,    // 4
		0b01101101,    // 5
		0b01111101,    // 6
		0b00000111,    // 7
		0b01111111,    // 8
		0b01101111,    // 9
		};

uint32_t writeByte(uint8_t data);
void startComm();
void stopComm();
void clrSegments();
void clrSegment(uint32_t segmentnr);
void setSegment(uint32_t zahl, uint32_t segment);

#endif /* SIEBENSEGMENT_TM1637_H_ */
