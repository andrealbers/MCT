/**
 * \file main.h
 *
 * \authors André Albers, Gustav Hey
 *
 * \date 12. November 2020
 *
 * \version 1.1
 * Auslesen der Taster T1-T4 ausschließlich dann, wenn Interrupt erfolgte
 *
 * \version 1.0
 *  Erstellung
 */

#ifndef MAIN_H_
#define MAIN_H_

//MAKROS
#define TA1 0b001      //TA1 gedrückt
#define TA2 0b010      //TA2 gedrückt
#define TA3 0b100      //TA3 gedrückt
#define TAnokey 0b000  //Zustand, wenn kein TA-Taster gedrückt wurde

#define L1 0           //Abstand L1
#define L2 1           //Abstand L2
#define L3 2           //Abstand L3
#define RESET_DATA 3   //Nur zum Reset
#define L1_1 4         //Wird für Alarmfunktion verwendet

#define UNTER 0  //Zustand für Alarmtyp unterschreiten
#define UEBER 1  //Zustand für Alarmtyp überschreiten

#define startwert 600    //Lautsprecher
#define kstartwert 200   //Lautsprecher
#define abtastzeit 1000  //Lautsprecher

#define Leerzeile "                    "   //Um eine Zeile des LCDs zu löschen (20 Zeichen)


//PROTOTYPEN
void io_init(void);
void delay(volatile uint32_t dtime);
void Tkeyhandler(uint32_t *i2ckeys, uint32_t *zu_erf_abstand);
uint32_t getTkeys();
void writeFrontLED(uint32_t led);
void lcdhandler(uint32_t *zustand, uint32_t *abstand, uint32_t *ZustandI2CLEDs);
uint32_t getTAkeys();
void TAkeyhandler(uint32_t *TAzustand, uint32_t *alrm_on, uint32_t *alrm_typ);
void saveDistance(uint32_t *abstande, uint32_t *zu_erf_abstand, uint32_t *ZustandFrontLEDs);
void writeSpeaker();
void distanzalarmHandler(uint32_t *abstande, uint32_t *alarmtyp);
int main(void);

#endif /* MAIN_H_ */
