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
#include "HC_SR04.h"
//#include "Siebensegment_TM1637.h"
#include "main.h"
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

	pinMode(Siebensegment_CLKpin, Siebensegmentport, OUTPUT);
	pinMode(Siebensegment_DIOpin, Siebensegmentport, OUTPUT);
}

void delay(volatile uint32_t dtime) { /** \param dtime Wartezeit */
	dtime = dtime * 100;
	while (dtime--)
		;
}

void keyhandler(uint32_t *i2ckeys, uint32_t *zu_erf_abstand) {
	switch (*i2ckeys) {
	case T1:
		*zu_erf_abstand = L1;
		break;
	case T2:
		*zu_erf_abstand = L2;
		break;
	case T3:
		*zu_erf_abstand = L3;
		break;
	case T4:
		*zu_erf_abstand = RESET_DATA;
		break;
	default:
		break;
	}
}

uint32_t geti2cKeys() {
	uint32_t i2ckeys;

	i2c_start();
	i2c_write_byte(I2C_PCF8574A_ADDR | I2C_READ);
	i2ckeys = i2c_read_byte(0);
	i2c_stop();
	i2ckeys = i2ckeys >> 4;
	i2ckeys &= (0b1111);
	return i2ckeys;
}

void writei2cLED(uint32_t led) {
	i2c_start();
	i2c_write_byte(I2C_PCF8574A_ADDR | I2C_WRITE);
	i2c_write_byte(led);
	i2c_stop();
}

void lcdhandler(uint32_t *zustand, uint32_t *abstand) {

	switch (*zustand) {
	case L1:
		writei2cLED(I2CLED1);
		lcd_write_string_xy("L1=", 1, 1);
		lcd_write_uint(abstand[L1], 4);
		lcd_write_string_xy("cm", 8, 1);
		break;
	case L2:
		writei2cLED(I2CLED1 & I2CLED2);
		lcd_write_string_xy("L2=", 1, 2);
		lcd_write_uint(abstand[L2], 4);
		lcd_write_string_xy("cm. A=", 8, 2);

		lcd_write_uint((abstand[L1] * abstand[L2]) / 10000, 5);
		lcd_write_string_xy("qm", 19, 2);
		break;
	case L3:
		writei2cLED(I2CLED1 & I2CLED2 & I2CLED3);
		lcd_write_string_xy("L3=", 1, 3);
		lcd_write_uint(abstand[L3], 4);
		lcd_write_string_xy("cm. V=", 8, 3);

		lcd_write_uint((abstand[L1] * abstand[L2] * abstand[L3]) / 1000000, 4);
		lcd_write_string_xy("cbm", 18, 3);
		break;
	case RESET_DATA:


		lcd_write_string_xy("Reset? T3: JA", 1, 4);

		uint32_t key = 0;
		do {
			delay(10);
			key = geti2cKeys();
		} while (key == 0b1111);         //Warte bis eine Eingabe erfolgte

		if (key == T3) {
			abstand[L1] = 0;
			abstand[L2] = 0;
			abstand[L3] = 0;
			lcd_clrscr();
			clrSegments();
			writei2cLED(CLR_I2CLED);
		}
		lcd_write_string_xy(Leerzeile, 1, 4);
	default:
		break;
	}

}

int main(void) {
	uint32_t zu_erf_abstand = L1, i2ckeys = 0b1111, zi2ckeys = 0b1111;
	uint32_t abstande[3] = { 0, 0, 0 };

	i2c_init(I2C_SM);
	lcd_init(4, I2C_SM);
	lcd_cursor(2);

	io_init(); //I/O-Ports initialisieren

	clrSegments();
	writei2cLED(CLR_I2CLED);

	while (1) {
		i2ckeys = geti2cKeys();

		if ((i2ckeys != zi2ckeys) && (i2ckeys != 0b1111)) {
			keyhandler(&i2ckeys, &zu_erf_abstand);

			if ((zu_erf_abstand == L1)
					|| ((zu_erf_abstand == L2) && (abstande[L1] != 0))
					|| ((zu_erf_abstand == L3) && (abstande[L1] != 0)
							&& (abstande[L2] != 0))) {
				abstande[zu_erf_abstand] = getDistance();
				setinttoSegment(abstande[zu_erf_abstand]); //Jetzt auf das Siebensegment mappen
				lcdhandler(&zu_erf_abstand, &abstande);

			}
			if (zu_erf_abstand == RESET_DATA)
				lcdhandler(&zu_erf_abstand, &abstande);
		}
		zi2ckeys = i2ckeys;
		delay(100);
	}

	i2c_exit();
	return 0;
}
