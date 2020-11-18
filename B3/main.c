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
#include "PCA_9539.h"
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

	pinMode(RGB_R, RGB_Rport, OUTPUT);
	pinMode(RGB_G, RGB_GBport, OUTPUT);
	pinMode(RGB_B, RGB_GBport, OUTPUT);

	pinMode(Speakerpin, Speakerport, OUTPUT); //Kein PWM möglich?

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
		break;
	case L1_1:
		lcd_write_string_xy("ALRM:", 1, 4);
		lcd_write_string_xy("L1=", 12, 4);
		lcd_write_uint(abstand[L1_1], 4);
		lcd_write_string_xy("cm", 19, 4);
		break;
	default:
		break;
	}

}

uint32_t getTAkeys() {   //Wenn Taster gedrückt -> digitalRead = LOW
	uint32_t TAzustand = 0b000;
	TAzustand |= ((!digitalRead(TA1pin, TAport)) << 0);
	TAzustand |= ((!digitalRead(TA2pin, TAport)) << 1);
	TAzustand |= ((!digitalRead(TA3pin, TAport)) << 2);
	return TAzustand;
}

void distanzalarm(uint32_t *TAzustand, uint32_t *alrm_on, uint32_t *alrm_typ) {

	switch (*TAzustand) {
	case TA1:
		*alrm_on = HIGH;
		*alrm_typ = UNTER;
		lcd_write_string_xy("unter", 6, 4);
		digitalWrite(RGB_R, RGB_Rport, RGB_EIN);
		digitalWrite(RGB_G, RGB_GBport, RGB_EIN);
		break;
	case TA2:
		*alrm_on = HIGH;
		*alrm_typ = UEBER;
		lcd_write_string_xy("ueber", 6, 4);
		digitalWrite(RGB_R, RGB_Rport, RGB_AUS);
		digitalWrite(RGB_G, RGB_GBport, RGB_EIN);
		break;
	case TA3:
		*alrm_on = LOW;
		digitalWrite(RGB_R, RGB_Rport, RGB_AUS);
		digitalWrite(RGB_G, RGB_GBport, RGB_AUS);
		digitalWrite(RGB_B, RGB_GBport, RGB_AUS);
		lcd_write_string_xy(Leerzeile, 1, 4);
		break;
	}
}

void mainRoutine(uint32_t *abstande, uint32_t *zu_erf_abstand) {
	abstande[*zu_erf_abstand] = getDistance();
	setinttoSegment(abstande[*zu_erf_abstand]); //Jetzt auf das Siebensegment mappen
	lcdhandler(zu_erf_abstand, abstande);

}

void setSpeaker() {

	for (uint32_t i = startwert; i < abtastzeit; i++) {
		digitalWrite(Speakerpin, Speakerport, HIGH);
		uint32_t k = kstartwert;
		while (k < i) {
			k++;
		}
		digitalWrite(Speakerpin, Speakerport, LOW);
		k = kstartwert;
		while (k < i) {
			k++;
		}
	}

	for (uint32_t i = abtastzeit; i > startwert; i--) {
		digitalWrite(Speakerpin, Speakerport, HIGH);

		uint32_t k = kstartwert;
		while (k < i) {
			k++;
		}

		digitalWrite(Speakerpin, Speakerport, LOW);
		k = kstartwert;
		while (k < i) {
			k++;
		}
	}

}

void distanzalarmHandler(uint32_t *abstande, uint32_t *alarmtyp) {

	switch (*alarmtyp) {
	case UNTER:  //TA1 wurde gedrueckt
		if (abstande[L1_1] < abstande[L1]) {
			writei2cRedLED(i2cRoteLEDsAN);
			delay(200);
			writei2cRedLED(i2cRoteLEDsAUS);
			setSpeaker();
		}
		break;
	case UEBER: //TA2 wurde gedrueckt
		if (abstande[L1_1] > abstande[L1]) {
			writei2cRedLED(i2cRoteLEDsAN);
			delay(200);
			writei2cRedLED(i2cRoteLEDsAUS);
			setSpeaker();
		}
		break;
	default:
		break;
	}

}

int main(void) {
	uint32_t zu_erf_abstand = L1, i2ckeys = 0b1111, zi2ckeys = 0b1111, TAkeys =
			0b000, zTAkeys = 0b000, alrm_on = LOW, alrm_typ = UNTER;
	uint32_t abstande[5] = { 0, 0, 0, 0, 0 };

	i2c_init(I2C_SM);
	lcd_init(4, I2C_SM);
	lcd_cursor(2);
	writei2cRedLED(i2cRoteLEDsAUS);
	io_init(); //I/O-Ports initialisieren
	clrSegments();
	writei2cLED(CLR_I2CLED);
	digitalWrite(RGB_R, RGB_Rport, RGB_AUS);
	digitalWrite(RGB_G, RGB_GBport, RGB_AUS);
	digitalWrite(RGB_B, RGB_GBport, RGB_AUS);
	timer_init();

	while (1) {
		i2ckeys = geti2cKeys();
		TAkeys = getTAkeys();

		if ((i2ckeys != zi2ckeys) && (i2ckeys != 0b1111)) {
			keyhandler(&i2ckeys, &zu_erf_abstand);

			if ((zu_erf_abstand == L1)
					|| ((zu_erf_abstand == L2) && (abstande[L1] != 0))
					|| ((zu_erf_abstand == L3) && (abstande[L1] != 0)
							&& (abstande[L2] != 0))) {
				mainRoutine(abstande, &zu_erf_abstand);
			}

			if (zu_erf_abstand == RESET_DATA)
				lcdhandler(&zu_erf_abstand, abstande);
		}

		if (((TAkeys != zTAkeys) && (TAkeys != 0b000) && (abstande[L1] != 0))
				|| alrm_on == HIGH) {
			distanzalarm(&TAkeys, &alrm_on, &alrm_typ);
			if (alrm_on == HIGH) {
				zu_erf_abstand = L1_1;
				mainRoutine(abstande, &zu_erf_abstand);

				distanzalarmHandler(abstande, &alrm_typ);

			}
		}

		zi2ckeys = i2ckeys;
		zTAkeys = TAkeys;
		delay(10);
	}

	i2c_exit();
	return 0;
}
