/*
 * HC_SR04.h
 *
 *  Created on: 17.11.2020
 *      Author: andre
 */

#ifndef HC_SR04_H_
#define HC_SR04_H_

#define PCRIT 1 << 16 //Power Control -> Peripherie aktivieren
#define RITEN 1 << 3  //Timer aktivieren/deaktivieren -> 1 = Aktivieren , 0 = Deaktivieren
#define PCLK_RIT 0b01 << 26 //Timer Clock auf  CCLK setzen

#define schallgeschwindigkeit 343.0 //in m/s


uint32_t getDistance(void);

#endif /* HC_SR04_H_ */
