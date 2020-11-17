#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif
#include "Siebensegment_TM1637.h"
#include "GPIO.h"
#include "lcdlib_1769.h"

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

void startComm() {
	//Start Verbindung
	digitalWrite(Siebensegment_CLKpin, Siebensegmentport, HIGH);
	delay(bitdelay);
	digitalWrite(Siebensegment_DIOpin, Siebensegmentport, HIGH);
	delay(bitdelay);
	digitalWrite(Siebensegment_DIOpin, Siebensegmentport, LOW);
	delay(bitdelay);
}

void stopComm() {
	//Stoppe Verbindung
	digitalWrite(Siebensegment_CLKpin, Siebensegmentport, HIGH);
	delay(bitdelay);
	digitalWrite(Siebensegment_DIOpin, Siebensegmentport, LOW);
	delay(bitdelay);
	digitalWrite(Siebensegment_DIOpin, Siebensegmentport, HIGH);
	delay(bitdelay);
}

void clrSegments() {
	uint32_t ack = 1;
	for (int i = 0; i < 4; i++) {
		startComm();
		ack = writeByte(wrDataToReg); //Setzen um Daten zum display register zu senden, ack = 1, wenn erfolgreich
		stopComm();

		startComm();
		ack = writeByte(segmente[i]); //Adresse setzen für Segment
		ack = writeByte(0b00000000);
		stopComm();

		startComm();
		ack = writeByte(DisplayOFF);  //Helligkeit setzen
		stopComm();
	}
}

void clrSegment(uint32_t segmentnr) {
	uint32_t ack = 1;

	startComm();
	ack = writeByte(wrDataToReg); //Setzen um Daten zum display register zu senden, ack = 1, wenn erfolgreich
	stopComm();

	startComm();
	ack = writeByte(segmente[segmentnr]); //Adresse setzen für Segment
	ack = writeByte(0b00000000);
	stopComm();

	startComm();
	ack = writeByte(DisplayOFF); //Helligkeit setzen
	stopComm();

}

void setSegment(uint32_t zahl, uint32_t segment) {
	uint32_t ack = 1;

	startComm();
	ack = writeByte(wrDataToReg); //Setzen um Daten zum display register zu senden, ack = 1, wenn erfolgreich
	stopComm();

	startComm();
	ack = writeByte(segmente[segment]); //Adresse setzen für Segment
	ack = writeByte(zahlen[zahl]);
	stopComm();

	startComm();
	ack = writeByte(DisplayON);  //Helligkeit setzen
	stopComm();

}

void setinttoSegment(uint32_t zahl) {
	clrSegments();

	uint32_t _zahl = zahl, posziffer = 0;

	while (_zahl != 0) {
		_zahl = _zahl / 10;
		posziffer++;
	}

	uint32_t anzahlZahlen[posziffer];

	posziffer = 0;
	_zahl = zahl;

	while (_zahl != 0) {
		anzahlZahlen[posziffer] = _zahl % 10;
		_zahl = _zahl / 10;
		posziffer++;
	}

	for (uint32_t i = 0, k = 3; i < posziffer; i++) { //1. segment=3 mit niedrigsten posziffer
		setSegment(anzahlZahlen[i], k);
		k--;
	}
}
