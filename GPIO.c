/**
 *
 * @param port
 * @param bit
 * @param type
 */

#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif
#include "GPIO.h"

void pinMode(uint32_t pin, uint32_t portnr, uint32_t mode) {
	unsigned int bit = 1 << pin;
	unsigned int dbl_pin = (bit << (pin + 1)) + bit; //Aus z.B. 100 -> 100 100
	dbl_pin = dbl_pin << 2 * pin;  //Aus z.B. x<<3 -> x<<6

	switch (portnr) {
	case 0:
		if (pin <= 15) {
			LPC_PINCON->PINSEL0 &= ~(dbl_pin);     //Als GPIOs setzen -> Bits 00
			LPC_PINCON->PINMODE0 &= ~(dbl_pin);          //Erstmal Bits löschen
			LPC_PINCON->PINMODE0 |= mode << (pin * 2);        //Pinmode setzen
		} else if (pin <= 31) {
			LPC_PINCON->PINSEL1 &= ~(dbl_pin);
			LPC_PINCON->PINMODE1 &= ~(dbl_pin);
			LPC_PINCON->PINMODE1 |= mode << (pin * 2);
		}
		if (mode == OUTPUT)
			LPC_GPIO0->FIODIR |= (1 << pin);
		else
			LPC_GPIO0->FIODIR &= ~(1 << pin);
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

void digitalWrite(uint32_t pin, uint32_t port, uint32_t set) {
	if (set == HIGH)
		LPC_GPIO[port]->FIOSET = (1 << pin);
	else
		LPC_GPIO[port]->FIOCLR = (1 << pin);
}

uint32_t digitalRead(uint32_t pin, uint32_t port) {
	uint32_t var;

	var = LPC_GPIO[port]->FIOPIN; //Alle Zustände von GPIO[*port] einlesen (32bit)
	var &= (1 << pin); //Nur den einen Zustand am Pin auslesen

	if (var > 0)
		var = HIGH;
	else
		var = LOW;

	return var;
}
