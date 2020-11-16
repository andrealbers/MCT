/*
 ===============================================================================
 Name        : B3_Abstandsmessung.c
 Author      : $(author)
 Version     :
 Copyright   : $(copyright)
 Description : main definition
 ===============================================================================
 */

#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif

#include <cr_section_macros.h>
#include <stdio.h>
#include "GPIO.h"
#include "lcdlib_1769.h"
#include "i2c_1769.h"

void io_init(void) {   //PIN, PORT; MODE (PULLUP, PULLDOWN, OUTPUT)
	//EINGÄNGE
	pinMode(TA1pin, TAport, PULLUP);
	pinMode(TA2pin, TAport, PULLUP);
	pinMode(TA3pin, TAport, PULLUP);

	pinMode(HC_Echopin, HCport, PULLUP);

	//AUSGÄNGE
	pinMode(LED0pin, yLEDport, OUTPUT);
	pinMode(LED1pin, yLEDport, OUTPUT);
	pinMode(LED2pin, yLEDport, OUTPUT);
	pinMode(LED3pin, yLEDport, OUTPUT);
	pinMode(LED4pin, yLEDport, OUTPUT);
	pinMode(LED5pin, yLEDport, OUTPUT);
	pinMode(LED6pin, yLEDport, OUTPUT);
	pinMode(LED7pin, yLEDport, OUTPUT);

	pinMode(HC_Triggerpin, HCport, OUTPUT);
}

void delay(volatile uint32_t dtime) { /** \param dtime Wartezeit */
	dtime = dtime * 100;
	while (dtime--)
		;
}

void keyhandler(uint32_t *ZustandKeys) {

}

int main(void) {

	i2c_init(I2C_SM);
	io_init(); //I/O-Ports initialisieren
	lcd_init(4, I2C_SM);
	lcd_cursor(2);

	//digitalWrite(LED7pin, yLEDport, LOW);
	//uint32_t key = digitalRead(TA3pin, TAport);

	while (1) {

		if (digitalRead(TA1pin, TAport) == LOW) {
			lcd_write_uint(getDistance(), 5);
			delay(50000);
			lcd_clrscr();
		}

	}

	i2c_exit();
	return 0;
}
