/**
 *  \file Siebensegment_TM1637.c
 */

#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif
#include "Siebensegment_TM1637.h"
#include "GPIO.h"
#include "lcdlib_1769.h"
#include "main.h"

/**
 * \brief <b>Ein Byte zum Siebensegment-Treiber TM1637 senden</b><br>
 * Es wird der Wert von der aufrufenden Funktion in data abgelegt
 * - Data-Leitung wird am Anfang als OUTPUT gesetzt, da der Treiber die Daten-Leitung auf LOW schaltet, wenn ein ACK erfolgte (Umschalten auf PULLUP notwendig)
 * - Um 8-Bit von der Variable "data" zu verarbeiten wird folgender Ablauf 8x durchlaufen
 *  - CLK wird auf LOW geschaltet
 *  - Wenn erstes Bit von data gleich 1 ist:
 *   - DIO wird auf HIGH geschaltet
 *  - sonst (muss 0 sein..)
 *   - DIO wird auf LOW geschaltet
 *  - CLK wird auf HIGH geschaltet -> Treiber übernimmt Wert in das interne Datenregister
 *  - Variable "data" wird um 1 nach rechts geshiftet, um das nächste Bit der Variable zu verarbeiten
 * - Es wird ein weiterer Takt erzeugt (CLK=LOW->CLK=HIGH)
 * - Schalten von der Datenleitung als PULLUP
 * - Wenn nun die Datenleitung den LOW-Pegel angenommen hat, hat der Treiber die Daten erfolgreich übernommen -> ACK
 * @param data Zu übergebendes Byte an Treiber
 * @return
 */
uint32_t writeByte(uint8_t data) {
	uint32_t ack = 0;

	pinMode(Siebensegment_DIOpin, Siebensegmentport, OUTPUT);

	//Übertrage Daten (1 Byte), CLK ist HIGH, DIO ist LOW von startComm
	for (uint32_t i = 0; i < 8; i++) {

		digitalWrite(Siebensegment_CLKpin, Siebensegmentport, LOW); //CLK LOW setzen

		delay(bitdelay);

		//Jetzt DIO für das erste Bit setzen
		if ((data & 0b1) == HIGH) {
			digitalWrite(Siebensegment_DIOpin, Siebensegmentport, HIGH);
		} else {
			digitalWrite(Siebensegment_DIOpin, Siebensegmentport, LOW);
		}

		delay(bitdelay);

		digitalWrite(Siebensegment_CLKpin, Siebensegmentport, HIGH); //CLK HIGH -> Bit einlesen
		delay(bitdelay);
		data = data >> 1;
	}

	digitalWrite(Siebensegment_CLKpin, Siebensegmentport, LOW);

	delay(bitdelay);

	digitalWrite(Siebensegment_CLKpin, Siebensegmentport, HIGH);

	//delay(bitdelay);

	pinMode(Siebensegment_DIOpin, Siebensegmentport, PULLUP);

	delay(bitdelay);

	ack = digitalRead(Siebensegment_DIOpin, Siebensegmentport); //ACK angekommen?

	digitalWrite(Siebensegment_CLKpin, Siebensegmentport, LOW);

	return ack;
}

/**
 * \brief <b>Starte Verbindung zum TM1637</b><br>
 * Die Verbindung zum Treiber erfolgt indem:
 * - CLK ist HIGH, während DIO von HIGH auf LOW wechselt
 */
void startComm() {
	//Start Verbindung
	digitalWrite(Siebensegment_CLKpin, Siebensegmentport, HIGH);
	delay(bitdelay);
	digitalWrite(Siebensegment_DIOpin, Siebensegmentport, HIGH);
	delay(bitdelay);
	digitalWrite(Siebensegment_DIOpin, Siebensegmentport, LOW);
	delay(bitdelay);
}

/**
 * \brief <b>Stoppe Verbindung zum TM1637</b><br>
 * Die Verbindung zum Treiber wird gestoppt indem:
 * - CLK ist HIGH, während DIO von LOW auf HIGH wechselt
 */
void stopComm() {
	//Stoppe Verbindung
	digitalWrite(Siebensegment_CLKpin, Siebensegmentport, HIGH);
	delay(bitdelay);
	digitalWrite(Siebensegment_DIOpin, Siebensegmentport, LOW);
	delay(bitdelay);
	digitalWrite(Siebensegment_DIOpin, Siebensegmentport, HIGH);
	delay(bitdelay);
}

/**
 * \brief <b>Jedes Segment aller Siebensegmentanzeigen ausschalten</b><br>
 * - Für jedes Segment (4 Stück) erfolgt folgender Durchlauf:
 *  - Starte Verbindung zum Treiber
 *  - Setze Byte für das Schreiben von Daten in das Display-Register (wrDataToReg)
 *  - Stoppe Verbindung
 *  - Starte Verbindung
 *  - Setze Byte für die Adressierung des Displays. Die Adressen der 4 Siebensegmente sind in dem Array "segmente" abgelegt. Hier wird der Index des Arrays
 *  für jeden Durchlauf erhöht
 *  - Schreibe 0 in das Register des enstprechenden Displays (alle einzelnen Segmente ausschalten)
 *  - Stoppe Verbindung
 */
void clrSegments() {
	const uint32_t segmente[] = { 0b11000000,   //1
			0b11000001,   //2
			0b11000010,   //3
			0b11000011,   //4
			};

//	uint32_t ack = 1;

	for (int i = 0; i < 4; i++) {
		startComm();
		writeByte(wrDataToReg); //Setzen um Daten zum display register zu senden
		stopComm();

		startComm();
		writeByte(segmente[i]); //Adresse setzen für Segment
		writeByte(0x0);
		stopComm();
		/*
		 startComm();
		 ack = writeByte(DisplayOFF);  //Helligkeit setzen, nicht nötig, da vorher alle Segmentteile ausgeschaltet werden..
		 stopComm();
		 */
	}
}

/**
 * \brief <b>Einzelnes Segment ausschalten, Nummer wird von der aufrufenden Funktion übergeben</b><br>
 *  - Starte Verbindung zum Treiber
 *  - Setze Byte für das Schreiben von Daten in das Display-Register (wrDataToReg)
 *  - Stoppe Verbindung
 *  - Starte Verbindung
 *  - Setze Byte für die Adressierung des Displays. Die Segmentnummer wird als Index in dem array "segmente" verwendet, da der Inhalt dessen, der Adresse enstpricht.
 *  - Schreibe 0 in das Register -> alle einzelnen Segmente ausschalten
 * @param segmentnr Das zu löschende Segment
 */
void clrSegment(uint32_t segmentnr) {
	const uint32_t segmente[] = { 0b11000000,   //1
			0b11000001,   //2
			0b11000010,   //3
			0b11000011,   //4
			};

	//uint32_t ack = 1;

	startComm();
	writeByte(wrDataToReg); //Setzen um Daten zum display register zu senden
	stopComm();

	startComm();
	writeByte(segmente[segmentnr]); //Adresse setzen für Segment
	writeByte(0x0);
	stopComm();

	/*
	 startComm();
	 ack = writeByte(DisplayOFF); //Helligkeit setzen, nicht nötig, da vorher alle Segmentteile ausgeschaltet werden..
	 stopComm();
	 */
}

/**
 * \brief <b>Schalte Dots ein</b>
 */
void setDots() {
	startComm();
	writeByte(wrDataToReg); //Setzen um Daten zum display register zu senden
	stopComm();

	startComm();
	writeByte(0b11000001); //Zweites Segment beschreiben
	writeByte(0b10000000); //Dot setzen
	stopComm();
}

/**
 * \brief <b>Zeige übergebende Zahl an das übergebende Segment dar</b><br>
 *  - Starte Verbindung zum Treiber
 *  - Setze Byte für das Schreiben von Daten in das Display-Register (wrDataToReg)
 *  - Stoppe Verbindung
 *  - Starte Verbindung
 *  - Setze Byte für die Adressierung des Displays. Die Segmentnummer wird als Index in dem array "segmente" verwendet, da der Inhalt dessen, der Adresse enstpricht.
 *  - Setze Byte für die entsprechenden einzelnen Segmente. Die Bitkombination für alle einzelnen Zahlen wird in dem Array "zahlen" gespeichert. Der Index entspricht
 *  der darzustellenden Zahl
 *  - Stoppe Verbindung
 * @param zahl Die abzubildende Zahl
 * @param segment Segment auf dem die Zahl abgebildet werden soll
 */
void setSegment(uint32_t zahl, uint32_t segment) {
	const uint32_t segmente[] = { 0b11000000,   //1
			0b11000001,   //2
			0b11000010,   //3
			0b11000011,   //4
			};

	const uint32_t zahlen[10] = { 0b00111111,    // 0
			0b00000110,    // 1
			0b01011011,    // 2
			0b01001111,    // 3
			0b01100110,    // 4
			0b01101101,    // 5
			0b01111101,    // 6
			0b00000111,    // 7
			0b01111111,    // 8
			0b01101111,    // 9
			};

	//uint32_t ack = 1;

	startComm();
	writeByte(wrDataToReg); //Setzen um Daten zum display register zu senden, ack = 1, wenn erfolgreich
	stopComm();

	startComm();
	writeByte(segmente[segment]); //Adresse setzen für Segment
	writeByte(zahlen[zahl]);
	stopComm();

	startComm();
	writeByte(DisplayON);
	stopComm();

}

/**
 * \brief <b>Die zu übergebende Zahl wird auf den 4 Siebensegmenten dargestellt</b><br>
 * - Alle Segmente löschen
 * - So lange die zu überbende Zahl durch 10 teilen, bis diese 0 entspricht -> Anzahl der Stellen wird ermittelt
 * - Array erstellen, dessen Größe die Anzahl der Stellen entspricht
 * - Die zu übergebende Zahl wird mit Modulo 10 geteilt -> z.B. 532 -> 53mod10 = 2
 *  - Das Ergebnis daraus entspricht der ersten Ziffer der Zahl und wird in dem Array "anzahlZahlen" gespeichert
 *  - Die zu übergebende Zahl wird durch 10 geteilt, um die nächste Ziffer an erster Stelle zu erhalten
 * - Dies wird so lange durchlaufen, solange die Zahl nicht 0 entspricht
 * - Nun besteht das Array "anzahlZahlen" aus den Ziffern der Zahl mit entsprechenden Index
 * - Abschließend wird setSegment aufgerufen, um für jeden Index des Arrays eine Zahl an das Siebensegment zu senden
 * @param zahl Die darzustellende Zahl als uint
 */
void setinttoSegment(uint32_t zahl) {
	clrSegments();

	uint32_t _zahl = zahl, posziffer = 0;

	while (_zahl != 0) {
		_zahl = _zahl / 10;  //532 -> 53 -> 5 -> 0
		posziffer++;
	}

	uint32_t anzahlZahlen[posziffer];

	posziffer = 0;
	_zahl = zahl;

	while (_zahl != 0) {
		anzahlZahlen[posziffer] = _zahl % 10; //532 -> 2 -> anzahlZahlen[0]=2 ...
		_zahl = _zahl / 10;
		posziffer++;
	}

	for (uint32_t i = 0, k = 3; i < posziffer; i++) { //1. segment=3 mit niedrigsten posziffer->Rechtes Segment
		setSegment(anzahlZahlen[i], k);
		k--;
	}
}
