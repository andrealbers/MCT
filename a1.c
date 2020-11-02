#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif

#include <cr_section_macros.h>
#include <stdio.h>
#include "a1.h"

void io_init(void) {
	LPC_PINCON->PINSEL0 &= ~(0b111111 << 12); //Bits 12 bis 17 von PINSEL0 auf 0 setzen für TASTER als GPIO
	LPC_PINCON->PINMODE0 &= ~(0b111111 << 12); //Taster GPIO als Pullup setzen -> Bits alle 0 setzen
	LPC_GPIO0->FIODIR &= ~(0b111 << 6); //Taster als Eingänge setzen -> Bits alle 0 setzen

	LPC_PINCON->PINSEL4 &= ~(0xFFFF << 2); //Bits 2 bis 17 von PINSEL4 aruf 0 setzen für LED als GPIO 0b1111 1111 1111 1111 << 2
	LPC_PINCON->PINMODE4 &= ~(0xFFFF << 2); //Nach Beschreibung soll PinMode der Leds auf Pull-Up gesetzt werden -> Bits alle 0 setzen
	LPC_GPIO2->FIODIR |= (0xFF << 1); //LEDs als Ausgang schalten -> Bits alle 1 setzen

	LPC_GPIO2->FIOSET = (0xFF << 1);  //LEDs ausschalten

	return;
}

void set_led(uint32_t pos) { //LED0-LED7 an PORT2 -> LED0 = P2.1 bis LED7 = P2.8

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
	return;
}

uint32_t getkey(void) {
	uint32_t gkey = 0;
	gkey = ~(LPC_GPIO0->FIOPIN >> 6); //TA1 zum LSB machen
	gkey &= ~(0xFFFFFFF8);            //Nur TA1-TA3 auslesen
	return gkey;
}

void delay(uint32_t dtime) {
	dtime = dtime * 10000;
	while (dtime--)
		;
	return;
}

void keyhandler(uint32_t keys, uint32_t *direction, uint32_t *runstate) {
	switch (keys) {
	case 1:
		*direction = LEFT; //TA1 gedrueckt -> Laufrichtung nach links
		break;
	case 2:
		*direction = RIGHT; //TA2 gedrueckt -> Laufrichtung nach rechts
		break;
	case 4:
	//	printf("keyhandler: case 4\n");
		if (*runstate == STOP)
			*runstate = START;
		else
			*runstate = STOP;
		break;
	default:
		break;
	}
	return;
}

int main(void) {
	uint32_t ledpos = 0;    //Position der aktiven LED (0..7)
	uint32_t key = 0;      //Tastencodes
	uint32_t run = STOP;  //Run-Mode (START | STOP)
	uint32_t dir = LEFT; //direction (LEFT | RIGHT)

	io_init(); //I/O-Ports initialisieren

	while (1) {
		set_led(ledpos); //LED an aktueller Position einschalten Übergabewert = Position der LED -> Pos0 bis Pos7

		key = getkey();  //Taster auslesen 1=Ta1, 2=Ta2, 4=Ta3

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
		delay(LEDTIME);
	}
	return 0;
}

