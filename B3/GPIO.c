/**
 *  \file GPIO.c
 */


#include "GPIO.h"

/**
 * \brief <b>Funktion um GPIO-Mode eines Pins zu setzen</b><br>
 * Je nach Port werden unterschiedliche Register angesteuert. Da man aus dem GPIO-Portpin nicht schließen kann, an welchem Port dieser angeschlossen ist,
 * wird dieser auch an die Funktion übergeben. <br>
 * Zudem werden für das Setzen des Pins als GPIO (PINSELX) und das Einstellen des Modus (PINMODEX) für den gleichen Port, verschiedene Register verwendet,
 * da für jeden Pin 2 Bits verwendet werden müssen und die Breite des Register auf 32 begrenzt ist. Daher ist auch eine Verdopplung des Verschiebens, sowie das Aufdoppeln
 * des Bitmusters notwendig. <br>
 * Folglich ist zwischen dem GPIO-Portpinbereich 0-15 und 16-31 zu unterscheiden, um so das richtige Register für PINSEL und PINMODE zu setzen. <br>
 * Dies gilt nicht für das Register FIODIR (Pin ist Eingang/Ausgang), da dieses nur 2 Zustände pro GPIO-Portpin annehmen kann.
 *
 * @param pin GPIO-Pin an dem der Mode gesetzt werden soll
 * @param portnr Port des GPIO-Pins
 * @param mode Mode der gesetzt werden soll; PULLUP, PULLDOWN, OUTPUT
 */
void pinMode(uint32_t pin, uint32_t portnr, uint32_t mode) {
	uint32_t dbl_pin = 1 << (2 * pin);    //1 << 6 -> 1 << 12
	dbl_pin |= 1 << ((2*pin)+1);          //1 << 12 -> 1 << 12, 1 << 13

	//uint32_t dbl_pin = (bit << (pin + 1)) + bit;  //Aus z.B. 10 -> 10 10; Wird für PINSEL und PINMODE benötigt

	switch (portnr) {
	case 0:
		if (pin <= 15) {
			LPC_PINCON->PINSEL0 &= ~(dbl_pin);          //Als GPIOs setzen -> Bits 00
			LPC_PINCON->PINMODE0 &= ~(dbl_pin);         //Erstmal Bits löschen
			LPC_PINCON->PINMODE0 |= mode << (pin * 2);  //Pinmode setzen -> Pullup, Pulldown, Output ...
		} else if (pin <= 31) {
			LPC_PINCON->PINSEL1 &= ~(dbl_pin);
			LPC_PINCON->PINMODE1 &= ~(dbl_pin);
			LPC_PINCON->PINMODE1 |= mode << (pin * 2);
		}
		if (mode == OUTPUT)
			LPC_GPIO0->FIODIR |= (1 << pin);            //Richtung des GPIO-Portpins auf Ausgang setzen
		else
			LPC_GPIO0->FIODIR &= ~(1 << pin);           //Richtung des GPIO-Portpins auf Eingang setzen
		break;

	case 1:
		if (pin <= 15) {
			LPC_PINCON->PINSEL2 &= ~(dbl_pin);
			LPC_PINCON->PINMODE2 &= ~(dbl_pin);
			LPC_PINCON->PINMODE2 |= mode << (pin * 2);
		} else if (pin <= 31) {
			LPC_PINCON->PINSEL3 &= ~(dbl_pin);
			LPC_PINCON->PINMODE3 &= ~(dbl_pin);
			LPC_PINCON->PINMODE3 |= mode << (pin * 2);
		}
		if (mode == OUTPUT)
			LPC_GPIO1->FIODIR |= (1 << pin);
		else
			LPC_GPIO1->FIODIR &= ~(1 << pin);
		break;

	case 2:
		if (pin <= 15) {
			LPC_PINCON->PINSEL4 &= ~(dbl_pin);
			LPC_PINCON->PINMODE4 &= ~(dbl_pin);
			LPC_PINCON->PINMODE4 |= mode << (pin * 2);
		} else if (pin <= 31) {
			LPC_PINCON->PINSEL5 &= ~(dbl_pin);
			LPC_PINCON->PINMODE5 &= ~(dbl_pin);
			LPC_PINCON->PINMODE5 |= mode << (pin * 2);
		}
		if (mode == OUTPUT)
			LPC_GPIO2->FIODIR |= (1 << pin);
		else
			LPC_GPIO2->FIODIR &= ~(1 << pin);
		break;

	case 3:
		if (pin <= 15) {
			LPC_PINCON->PINSEL6 &= ~(dbl_pin);
			LPC_PINCON->PINMODE6 &= ~(dbl_pin);
			LPC_PINCON->PINMODE6 |= mode << (pin * 2);
		} else if (pin <= 31) {
			LPC_PINCON->PINSEL7 &= ~(dbl_pin);
			LPC_PINCON->PINMODE7 &= ~(dbl_pin);
			LPC_PINCON->PINMODE7 |= mode << (pin * 2);
		}
		if (mode == OUTPUT)
			LPC_GPIO3->FIODIR |= (1 << pin);
		else
			LPC_GPIO3->FIODIR &= ~(1 << pin);
		break;

	case 4:
		if (pin <= 15) {
			LPC_PINCON->PINSEL8 &= ~(dbl_pin);
			LPC_PINCON->PINMODE8 &= ~(dbl_pin);
			LPC_PINCON->PINMODE8 |= mode << (pin * 2);
		} else if (pin <= 31) {
			LPC_PINCON->PINSEL9 &= ~(dbl_pin);
			LPC_PINCON->PINMODE9 &= ~(dbl_pin);
			LPC_PINCON->PINMODE9 |= mode << (pin * 2);
		}
		if (mode == OUTPUT)
			LPC_GPIO4->FIODIR |= (1 << pin);
		else
			LPC_GPIO4->FIODIR &= ~(1 << pin);
		break;
	}
}

/**
 * \brief <b>Setzen eines GPIO-Pinport auf HIGH bzw. LOW</b> <br>
 * @param pin Der zu setzende Pin
 * @param port Port an dem Pin angeschlossen ist
 * @param set Pin auf HIGH/LOW setzen
 */
void digitalWrite(uint32_t pin, uint32_t port, uint32_t set) {
	if (set == HIGH)                           //Wenn Ausgang HIGH geschaltet werden soll
		LPC_GPIO[port]->FIOSET = (1 << pin);
	else
		LPC_GPIO[port]->FIOCLR = (1 << pin);   //Wenn Ausgang LOW geschaltet werden soll
}

/**
 * \brief <b>Zustand des GPIO-Pinports auslesen</b> <br>
 * Hierbei werden alle Zustände des Ports ausgelesen und anschließend mit dem auszulesenden Pins verundet. <br>
 * Dadurch wird nur der Zustand am Pin augelesen. Als Rückgabewert erfolgt der Zustand kodiert als HIGH(1) oder LOW(0). <br>
 * @param pin Der auszulesende Pin
 * @param port Port an dem der Port angeschlossen ist
 * @return Zustand des GPIO-Pinports HIGH/LOW
 */
uint32_t digitalRead(uint32_t pin, uint32_t port) {
	uint32_t var;

	var = LPC_GPIO[port]->FIOPIN;    //Alle Zustände von GPIO[port] einlesen (32bit)
	var &= (1 << pin);               //Nur den einen Zustand am Pin auslesen
	var = var >> pin;                //Gesuchtes Bit zum LSB machen

	/*  //Nicht benötigt, da ich nun das gesuchte Bit zum LSB mache
	if (var > 0)
		var = HIGH;
	else
		var = LOW;
*/

	return var;
}

/**
 * \brief <b>RGB-LEDs auf dem Maiboard setzen</b> <br>
 *  @param rgb_led Die zu setzende LED - RGB_AUS schaltet alle 3 LEDs aus
 */
void set_rgb(uint32_t rgb_led) {      //Übergabewert besteht aus 3 bits -> 0bxxx -> Wobei jede Bit für eine LED steht
	//Alle 3 LEDs ausschalten
	digitalWrite(RGB_Rpin, RGB_Rport, HIGH);
	digitalWrite(RGB_Gpin, RGB_GBport, HIGH);
	digitalWrite(RGB_Bpin, RGB_GBport, HIGH);

	//Jeweilige RGB einschalten
	if(rgb_led & RGB_R) {
		digitalWrite(RGB_Rpin, RGB_Rport, LOW);
	}

	if (rgb_led & RGB_G) {
		digitalWrite(RGB_Gpin, RGB_GBport, LOW);
	}

	if (rgb_led & RGB_B) {
		digitalWrite(RGB_Bpin, RGB_GBport, LOW);
	}
}
