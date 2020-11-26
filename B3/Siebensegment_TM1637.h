/**
 *  \file Siebensegment_TM1637.h
 */

#ifndef SIEBENSEGMENT_TM1637_H_
#define SIEBENSEGMENT_TM1637_H_

#define wrDataToReg 0b01000000

#define DisplayOFF 0b10000000 //Display AUS
#define DisplayON  0b10001000 //Display AN

#define bitdelay 5 //Delay zwischen Umschalten Clk

extern const uint32_t segmente[4];
extern const uint32_t zahlen[10];


uint32_t writeByte(uint8_t data);
void startComm();
void stopComm();
void clrSegments();
void clrSegment(uint32_t segmentnr);
void setSegment(uint32_t zahl, uint32_t segment);
void setinttoSegment(uint32_t zahl);

#endif /* SIEBENSEGMENT_TM1637_H_ */
