/**
 *  \file a1.c
 */

/**
 * \mainpage A1: Lauflicht - von Gustav Hey (7012604) & André Albers (7012484)
 *
 * \section sec_1 Funktion des Programms
 * Lauflicht der orangen LEDs, gesteuert über Taster TA1-TA3 <br>
 * TA1 = Richtung des Lauflichtes: links <br>
 * TA2 = Richtung des Lauflichtes: rechts <br>
 * TA3 = Start/Stopp des Lauflichtes an beliebiger Position <br>
 *
 * \subsection sec1_1 Beschreibung der Submodule
 *   - <b>io_init:</b> Setzen und Konfiguration der Ports des Mikrocontrollers
 *          -# TA1 bis TA3 als Eingang setzen
 *          -# LED0 bis LED7 als Ausgang setzen
 *          -# LED0 bis LED7 ausschalten
 *   - <b>set_led:</b> Setzen der orangen LEDs an der Position des übergebenden Wertes
 *   		-# Überprüfen ob pos in zulässigem Bereich (0-7)
 *   		-# Wenn OK: LED0-LED7 ausschalten \n
 *   		            LED an gegebener Position einschalten (pos+1, da LEDs start bei P2.1 und nicht P2.0)
 *   		-# Wenn n. OK: Fehlermeldung ausgeben -> pos außerhalb Bereich
 *   - <b>getkey:</b> Zustand Taster TA1-TA3 auslesen und als int zurückgeben
 *   		-# Zustand von PORT0 auslesen und Bits um 6 nach rechts schieben, um TA1 -> LSB
 *   		-# TA1-TA3 mit 1 verunden -> restlichen Bits = 0 -> Nur TA1 bis TA3 werden ausgelesen
 *   		-# Rückgabewert von Zustand der Taster als int gkey
 *   - <b>delay:</b> Funktion um delay zu erzeugen
 *   - <b>keyhandler:</b> Je nach Zustand von TA1-TA3 den Ablauf der LEDs manipulieren
 *          -# Kopieren von Tasterzustand in keys, Adresse von direction empfangen, Adresse von runstate empfangen
 *          -# Wenn TA1 gedrückt: direction wird auf LINKS gesetzt
 *          -# Wenn TA2 gedrückt: direction wird auf RECHTS gesetzt
 *          -# Wenn TA3 gedrückt: Umkehren vom aktuellen Zustand runstate START/STOPP
 */

#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif

#include <cr_section_macros.h>
#include <stdio.h>
#include "a1.h"

/**
 * \brief <b>Setzen und Konfiguration der Ports des Mikrocontrollers </b>
 *  - TA1 bis TA3 als Eingang setzen
 *  - LED0 bis LED7 als Ausgang setzen
 *  - LED0 bis LED7 ausschalten
 */
void io_init(void) {

	LPC_PINCON->PINSEL0 &= ~(0b111111 << 12); // Bits 12 bis 17 von PINSEL0 auf 0 setzen für TASTER als GPIO
	LPC_PINCON->PINMODE0 &= ~(0b111111 << 12); //Taster GPIO als Pullup setzen -> Bits alle 0 setzen
	LPC_GPIO0->FIODIR &= ~(0b111 << 6); //Taster als Eingänge setzen -> Bits alle 0 setzen

	LPC_PINCON->PINSEL4 &= ~(0xFFFF << 2); //Bits 2 bis 17 von PINSEL4 auf 0 setzen für LED als GPIO 0b1111 1111 1111 1111 << 2
	LPC_PINCON->PINMODE4 &= ~(0xFFFF << 2); //Nach Beschreibung soll PinMode der Leds auf Pull-Up gesetzt werden -> Bits alle 0 setzen
	LPC_GPIO2->FIODIR |= (0xFF << 1); //LEDs als Ausgang schalten -> Bits alle 1 setzen

	LPC_GPIO2->FIOSET = (0xFF << 1);            //LEDs ausschalten
}

/**
 * \brief <b>Setzen der orangen LEDs an der Position des übergebenden Wertes</b>
 *  - Überprüfen ob pos in zulässigem Bereich (0-7)
 *  - Wenn OK: LED0-LED7 ausschalten \n
 *    LED an gegebener Position einschalten (pos+1, da LEDs start bei P2.1 und nicht P2.0)
 *  - Wenn n. OK: Fehlermeldung ausgeben -> pos außerhalb Bereich
 */
void set_led(uint32_t pos) { /** \param pos Die Position der einzuschaltenden LED */ //LED0-LED7 an PORT2 -> LED0 = P2.1 bis LED7 = P2.8

	if (pos <= 7 && pos >= 0) {
		LPC_GPIO2->FIOSET = (0xFF << 1);  //LEDs ausschalten
		LPC_GPIO2->FIOCLR = (1 << (pos + 1));
	} else
		printf("set_led: Übergabewert 'pos' außerhalb Bereich!\n");

	/*
	 while (1) {
	 for (int i = 1; i <= 8; i++) {
	 LPC_GPIO2->FIOCLR = (1 << i);
	 if (i >= 2)
	 LPC_GPIO2->FIOSET = (1 << (i - 1));
	 delay(3);
	 }
	 LPC_GPIO2->FIOSET = (1 << 8);
	 }
	 */
}

/**
 * \brief <b>Zustand Taster TA1 bis TA3 auslesen und als int zurückgeben</b>
 *  - Zustand von PORT0 auslesen und Bits um 6 nach rechts schieben, um TA1 -> LSB
 *  - TA1-TA3 mit 1 verunden -> restlichen Bits = 0 -> Nur TA1 bis TA3 werden ausgelesen
 *  - Rückgabewert von Zustand der Taster als int gkey
 */
uint32_t getkey(void) {
	uint32_t gkey = 0;
	gkey = ~(LPC_GPIO0->FIOPIN >> 6); //TA1 zum LSB machen
	gkey &= 0b111;                    //Nur TA1-TA3 auslesen
	return gkey; /** \return Zustand der Taster TA1-TA3 */

}

/**
 * \brief <b>Funktion um delay zu erzeugen</b>
 */
void delay(uint32_t dtime) { /** \param dtime Wartezeit */
	dtime = dtime * 10000;
	while (dtime--)
		;
}

/**
 * \brief <b>Je nach Zustand von TA1-TA3 den Ablauf der LEDs manipulieren</b>
 *  - Kopieren von Tasterzustand in keys, Adresse von direction empfangen, Adresse von runstate empfangen
 *  - Wenn TA1 gedrückt: direction wird auf LINKS gesetzt
 *  - Wenn TA2 gedrückt: direction wird auf RECHTS gesetzt
 *  - Wenn TA3 gedrückt: Umkehren vom aktuellen Zustand runstate START/STOPP
 */
void keyhandler(uint32_t keys, uint32_t *direction, uint32_t *runstate) { /** \param keys,  *direction,  *runstate <br> keys: Beinhaltet Tasterzustand <br> *direction: Adresse für Richtungszuweisung LEDs <br> *runstate: Adresse für Laufzustand der LEDs */
	switch (keys) {
	case TA1:
		*direction = LEFT; //TA1 gedrueckt -> Laufrichtung nach links
		break;
	case TA2:
		*direction = RIGHT; //TA2 gedrueckt -> Laufrichtung nach rechts
		break;
	case TA3:
		if (*runstate == STOP)
			*runstate = START;
		else
			*runstate = STOP;
		break;
	default:
		break;
	}
}

/**
 * \brief <b>Programmablauf Lauflicht ausführen</b>
 *  - I/O-Ports initialisieren
 *  - LED an definierter Position setzen
 *  - Taster auslesen
 *  - keyhandler aufrufen, wenn Tasterzustand geändert
 *  - Wenn im Zustand START und LEFT -> position erhöhen, bis die letzte LED erreicht ist. Wenn die letzte LED erreicht ist, erste LED ansteuern
 *  - Wenn im Zustand START und RIGHT -> position verringern, bis die erste LED erreicht ist. Wenn die erste LED erreicht ist, letzte LED ansteuern
 *  - Wenn NICHT im Zustand START -> Variable ledpos nicht ändern
 *  - Zum Schluss wird der letzte Zustand der Taster in zkey abgelegt
 */
int main(void) {
	uint32_t ledpos = 0;    //Position der aktiven LED (0..7)
	uint32_t key = 0;       //Tastencodes
	uint32_t run = STOP;    //Run-Mode (START | STOP)
	uint32_t dir = LEFT;    //direction (LEFT | RIGHT)
	uint32_t zkey = 0;	//Variable um den letzten Zustand von keys zu speichern

	io_init(); //I/O-Ports initialisieren

	while (1) {
		set_led(ledpos); //LED an aktueller Position einschalten Übergabewert = Position der LED -> Pos0 bis Pos7

		key = getkey();  //Taster auslesen 1=Ta1, 2=Ta2, 4=Ta3

		if (key != zkey)        //Wenn Taster sich ändern -> keyhandler aufrufen
			keyhandler(key, &dir, &run);  //gemäß Tastendruck agieren

		if (run == START) {   //wenn aktiv
			if (dir == LEFT) {
				if (ledpos < 7)
					ledpos++;
				else
					ledpos = 0;
			} else {
				if (ledpos > 0)
					ledpos--;
				else
					ledpos = 7;
			}
		}
		zkey = key;          //letzten Zustand key sichern
		delay(LEDTIME);
	}
	return 0;
}

