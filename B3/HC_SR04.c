/**
 *  \file HC_SR04.c
 */

#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif

#include "i2c_1769.h"
#include "GPIO.h"
#include "HC_SR04.h"
#include "main.h"

/**
 * \brief <b>Gleitkommazahl runden</b>
 * @param zahl Zahl die gerundet werden soll
 * @return Gerundete Zahl
 */
uint32_t runden(float zahl) {   //zahl z.B. 123,7cm
	return (int)(zahl+0.5);     //Zu float 0.5 addieren und als int zurückgeben
}

/**
 * \brief <b>Distanz zu einem Objekt bestimmen</b> <br>
 * Zeit bei dem das Signals vom Echopin auf HIGH ist, bestimmt Laufzeit des Ultraschallsignals
 * - Ultraschallsensor wird getriggert
 *  - Sobald der Echopin von LOW auf HIGH wechselt wird der RIT aktiviert
 *  - Der Timer läuft so lange, bis der Echopin den Zustand LOW einimmt
 *  - RIT deaktivieren und Laufzeit in dem Array "duration" speichern
 *  - Dieses Verfahren insgesamt 5x durchlaufen
 *  - Summe aller Laufzeiten wird im Array "duration" an Stelle 5 gespeichert
 * - Mittelwert von allen 5 Laufzeiten wird in "mittelDuration" abgelegt
 * - Diese Laufzeit wird halbiert, da die Laufzeit für den Hin- und Rückweg gilt
 * - Anschließend erfolgt die Umrechnung in µs. Ein Inkrement des Timers entspricht 10ns. Daher wird "mittelDuration mit (10ns/1000ns)*1µs multipliziert.
 * - Um nun diese Zeit in eine Entfernung umwandeln zu können, wird die Zeit (µs) mit der Schallgeschwindigkeit (in cm/µs) multipliziert.
 * - Nun wird der Abstand vom Datentyp float gerundet und als unsigned int + Gehäusebreite (6cm) an die aufrufende Funktion übergeben.
 * @return Entfernung zum Objekt in cm (mit Gehäusebreite)
 */
uint32_t getDistance(void) {             //Schallgeschwindigkeit in Luft bei 20°C -> 343m/s -> 34,3cm/ms
	uint32_t duration[6] = { 0, 0, 0, 0, 0, 0 };
	float mittelDuration = 0.0, usDuration = 0.0, distance = 0.0;
/*
	LPC_SC->PCONP |= PCRIT;          //Peripherie aktivieren
	LPC_SC->PCLKSEL1 &= ~(0b11 << 26);  //Erst löschen
	LPC_SC->PCLKSEL1 |= (PCLK_RIT);       //Dann Timer Clock auf CCLK setzen
	LPC_RIT->RICTRL &= ~(RITEN); //Timer deaktivieren
*/
	for (int i = 0; i <= 4; i++) {
		LPC_RIT->RICOUNTER = 0;
		digitalWrite(HC_Triggerpin, HCport, HIGH);       //Ultraschallsensor triggern (min 10µs Signal auf HIGH halten)
		delay(50);
		digitalWrite(HC_Triggerpin, HCport, LOW);        //Jetzt sendet der Ultraschallsensor einen Burst aus

		while (digitalRead(HC_Echopin, HCport) == LOW) {
			//Warten bis Signal auf High wechselt
		}

		LPC_RIT->RICTRL |= (RITEN); //Timer aktivieren

		while (digitalRead(HC_Echopin, HCport) == HIGH) {
			//Warten bis Signal wieder auf LOW wechselt -> Laufzeit = Zeit, wie lange Signal HIGH war
		}

		LPC_RIT->RICTRL &= ~(RITEN); //Timer deaktivieren

		duration[i] = LPC_RIT->RICOUNTER;         //Inkrementiert in 10ns pro Inkrement
		duration[5] = duration[5] + duration[i];
		delay(5000);                              //Bis zum erneuten triggern min 20ms warten
	}
	mittelDuration = duration[5] / 5;             //Mittelwert bestimmen
	mittelDuration = mittelDuration / 2;          //Nur Signallaufzeit hin bzw. zurück
	usDuration = ((float) mittelDuration) * (10.0 / 1000.0);  //Zeit in us die gebraucht wurde für hin bzw zurück (1/10ns) * (10ns/1000ns) -> µs
	distance = usDuration * ((schallgeschwindigkeit * 100.0) / 1000000.0);   //Schallgeschwindigkeit in m/s, Abstand in cm Hin bzw Zurück
//	distance = distance * 10;  //Ausgabe in mm..

	uint32_t rnd_abstand = runden(distance);

	return rnd_abstand+6;  //Gehäusebreite 6cm
}



