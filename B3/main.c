/**
 *  \file main.c
 */

/** \mainpage B3: Abstandsmessgerät mit Alarm- und Rechenfunktion - von André Albers (7012484) und Gustav Hey (7012604)
 *
 * \section sec_1 Funktion des Programms
 * Erfassung einer Distanz durch den Abstandssensor <b>HC-SR04</b> mit Darstellung des aktuell gemessenen Abstand auf einer Siebensegmentanzeige (Treiber: <b>TM1637</b>)
 * und den Funktionen zur Berechnung des <b>Flächeninhaltes</b>, sowie zur <b>Volumenberechnung</b>. Alle Informationen werden dazu auf dem <b>LC-Display</b>
 * dargestellt. Weiter ist es möglich eine <b>Alarmfunktion</b> zu aktivieren, die bei über- bzw. unterschreiten einer Distanz einen Alarm auslöst.<br><br>
 * Dabei lösen folgende <b>Tasten</b> folgende <b>Events</b> aus:<br>
 *  <b>T1:</b> Auslösen der Messung von Abstand L1 <br>
 *	<b>T2:</b> Auslösen der Messung von Abstand L2; Anzeige Flächeninhalt (L1*L2) <br>
 *	<b>T3:</b> Auslösen der Messung von Abstand L3; Anzeige Volumeninhalt (L1*L2*L3) <br>
 *	<b>T4:</b> Rücksetzen aller gespeicherten Abstände; Anschließend T3 zum Bestätigen des resets <br>
 *  <b>TA1:</b> Permanente Messung des Abstandes - Distanz wird in Zeile 4 des LCD angezeigt. Aktivieren des Alarms bei Unterschreitung des Abstands von L1 <br>
 *	<b>TA2:</b> Permanente Messung des Abstandes - Distanz wird in Zeile 4 des LCD angezeigt. Aktivieren des Alarms bei Überschreitung des Abstands von L1 <br>
 *	<b>TA3:</b> Deaktivierung der Alarmfunktion <br><br>
 *	Dabei zeigen folgende <b>Elemente</b> folgenden <b>Zustand</b> an:<br>
 *	<b>Siebensegment am Abstandmessgerätmoduls</b>: Anzeige des zuletzt gemessenen Abstandes in cm. Bei Alarmfunktion AKTIV:
 *	Anzeige des aktuell gemessenen Abstandes in cm. <br>
 *  <b>Rote LEDs am Frontpanel</b>: Wenn eingeschaltet: Abstand wurde bereits gemessen. Sowie bei LED2 und LED3: Die Berechnung des Flächeninhaltes bzw. des Volumeninhaltes
 *  ist aktuell (wenn nach bereits berechnetem Wert ein vorheriger Abstand erneut eingemessen wurde, ist die LED aus). <br>
 *	<b>RGB-LED</b>: <b>GELD</b> Alarmfunktion UNTERSCHREITEN aktiv. <b>GRÜN</b> Alarmfunktion ÜBERSCHREITEN aktiv. <br>
 *	<b>8 rote LEDs auf Mainboard</b>: LED Reihe blinkt, wenn ALARM aktiv (Über- bzw. Unterschreiten der Distanz L1). <br>
 *	<b>Lautsprecher</b>: Lautsprecher erzeugt Tonsignal, wenn ALARM aktiv (Über- bzw. Unterschreiten der Distanz L1).
 *
 *	\subsection sec1_1 Beschreibung der Submodule
 *	<b>main:</b> Hauptfunktion. Hier werden entsprechend der Eingabe des Users, enstsprechende Funktionen aufgerufen. <br>
 *	<b>io_init:</b> Konfiguration der GPIO-Pins als PULLUP, PULLDOWN oder OUTPUT. <br>
 *	<b>delay:</b> Erzeugen eines Delays. <br>
 *
 *	<b>getTAkeys:</b> TA1 bis TA3 einlesen und den Zustand an die aufrufende Funktion übergeben. <br>
 *	<b>TAkeyhandler:</b> Handler um auf einen Tastendruck von TA1 bis TA3 zu reagieren. <br>
 *	<b>Tkeyhandler:</b> Handler um auf einen Tastendruck von T1 bis T4 zu reagieren. <br>
 *
 *	<b>lcdhandler:</b> Ausgabe auf das LC-Display und setzen der Front-LEDs je nach den zu messenden Abstandes. <br>
 *	<b>saveDistance:</b> Speichern des gemessenden Abstandes in ein Array, sowie Siebensegment, LC-Display und Front-LEDs setzen. <br>
 *	<b>writeSpeaker:</b> Erzeugen eines Tonsignals (wird bei aktivieren Alarm aufgerufen). <br>
 *	<b>distanzalarmHandler:</b> Wenn die Alarmfunktion aktiv ist, wird diese Funktion aufgerufen. Hier erfolgt die Berechnung, ob der aktiv gemessende Abstand den Wert von
 *	L1 über- bzw. unterschreitet. Wenn ja, werden hier die LEDs ein- & ausgeschaltet und writeSpeaker aufgerufen. <br>
 *  <b>timer_init:</b> RIT aktivieren <br>
 *
 *  <b><u>GPIO</u></b> <br>
 *	<b>pinMode:</b> Setze den zu übergebenden Pin (+Portangabe) als PULLUP, PULLDOWN oder als OUTPUT. <br>
 *	<b>digitalWrite:</b> Setze den zu übergebenden Pin (+Portangabe) in den Zustand HIGH oder in LOW. <br>
 *	<b>digitalRead:</b> Erhalte den Zustand des zu übergebenden Pins (+Portangabe) als Rückgabewert. <br>
 **
 *	<b><u>PCF8574 (8-Bit-Portexpander, I2C <-> parallel)</u></b> <br>
 *	<b>getTkeys:</b> Zustand von T1 bis T4 einlesen und an aufrufende Funktion übergeben. <br>
 *	<b>writeFrontLED:</b> Ein- bzw. Ausschalten der an der Frontplatte befestigen LEDs. <br>
 *
 *	<b><u>TM1637 Siebensegmenttreiber</u></b> <br>
 *	<b>writeByte:</b> Funktion um ein Byte an den Siebensegmenttreiber TM1637 zu senden. <br>
 *	<b>startComm:</b> Starte Kommunikation mit dem Siebensegmenttreiber TM1637. <br>
 *	<b>stopComm:</b> Stoppe Kommunikation mit dem Siebensegmenttreiber TM1637. <br>
 *	<b>clrSegments:</b> Schalte jedes Segment der Siebensegmente aus. <br>
 *	<b>clrSegment:</b> Schalte jenes Segment von dem Siebensegment aus, welches als Nummer an Funktion übergeben wurde. <br>
 *	<b>setSegment:</b> Zeige eine Nummer auf ein Segment an, welches von Funktion übergeben wurde. <br>
 *	<b>setinttoSegment:</b> Empfange eine Zahl als int und zeige diese auf den Siebensegmenten an. <br><br>
 *
 *	<b><u>PCA9539 (16-Bit-Portexpander, I2C <-> parallel)</u></b> <br>
 *	<b>writei2cRedLED</b>: Schalte die linken 8 LEDs auf dem Mainboard ein bzw. aus. <br>
 *
 *	<b><u>HC-SR04 Abstandssensor</u></b> <br>
 *	<b>runden:</b> Runde zu übergebende Zahl auf- bzw ab. <br>
 *	<b>getDistance:</b> Erhalte gemessenden Abstand mit Gehäusebreite als Rückgabewert.<br>
 *

 */
#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif

#include <cr_section_macros.h>
#include <stdio.h>
#include "GPIO.h"
#include "HC_SR04.h"
//#include "Siebensegment_TM1637.h"
#include <PCA9539.h>
#include "PCF8574.h"
#include "main.h"
#include "lcdlib_1769.h"
#include "i2c_1769.h"

//INterrupt von i2c tastern

/**
 * \brief <b>Konfiguration der GPIO-Pins des Mikrocontrollers </b>
 */
void io_init(void) {   //PIN, PORT; MODE (PULLUP, PULLDOWN, OUTPUT)
	//EINGÄNGE
	pinMode(TA1pin, TAport, PULLUP);
	pinMode(TA2pin, TAport, PULLUP);
	pinMode(TA3pin, TAport, PULLUP);

	pinMode(HC_Echopin, HCport, PULLUP);

	pinMode(InterruptI2CFrontpin, InterruptI2CFrontport, PULLUP);

	//AUSGÄNGE
	pinMode(LED0pin, yLEDport, OUTPUT);  //Wird gar nicht gebraucht..
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

	pinMode(Speakerpin, Speakerport, OUTPUT);   //Kein PWM möglich?
}

/**
 * \brief <b>Funktion um delay zu erzeugen</b>
 * @param dtime Wartezeit
 */
void delay(volatile uint32_t dtime) {
	dtime = dtime * 100;
	while (dtime--)
		;
}

/**
 * \brief <b>Zustand der Taster TA1, TA2 und TA3 auslesen</b><br>
 * Der Zustand wird in TAzustand gespeichert, wobei jedes Bit einen Taster darstellt (s. main.h)
 * Da die Taster im Pullup-Netzwerk verwendet werden, wird der Eingang des GPIO auf Masse gezogen.
 * Dies hat zu folge, dass digitalRead LOW ausliest, wenn der Taster gedrückt wird.
 * Da ich aber den Zustand HIGH gerne als gedrückt interpretiere, wird der Rückgabewert von digitalRead hier negiert.
 * @return Zustand der Taster TA1-TA3
 */
uint32_t getTAkeys() {   //Wenn Taster gedrückt -> digitalRead = LOW
	uint32_t TAzustand = 0b000;
	TAzustand |= ((!digitalRead(TA1pin, TAport)) << 0);
	TAzustand |= ((!digitalRead(TA2pin, TAport)) << 1);
	TAzustand |= ((!digitalRead(TA3pin, TAport)) << 2);
	return TAzustand;
}

/**
 * \brief <b>Handler der Taster TA1-TA3</b><br>
 * - TA1 gedrückt:
 *  - Alarmzustand einschalten
 *  - Alarmtyp unterschreiten aktivieren
 *  - auf dem LC-Display in Zeile 4 das Wort "unter" schreiben
 *  - RGB-LED auf Gelb schalten <br>
 *
 * - TA2 gedrückt:
 *  - Alarmzustand einschalten
 *  - Alarmtyp überschreiten aktivieren
 *  - auf dem LC-Display in Zeile 4 das Wort "über" schreiben
 *  - RGB-LED auf Grün schalten
 *
 * - TA3 gedrückt:
 *  - Alarmzustand ausschalten
 *  - RGB-LED ausschalten
 *  - Text von Zeile 4 des LC-Display löschen
 *
 * @param TAzustand Zustand der Taster TA1-TA3
 * @param alrm_on Alarmzustand eingeschaltet/ausgeschaltet
 * @param alrm_typ Alarmtyp überschreiten/unterschreiten
 */
void TAkeyhandler(uint32_t *TAzustand, uint32_t *alrm_on, uint32_t *alrm_typ) {
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

/**
 * \brief <b>Handler der Taster T1-T4</b><br>
 * - T1 gedrückt:
 *  - der zu erfassende Abstand soll L1 sein
 * - T2 gedrückt:
 *  - der zu erfassende Abstand soll L2 sein
 * - T3 gedrückt:
 *  - der zu erfassende Abstand soll L3 sein
 * - T4 gedrückt:
 *  - Resetmodus wird aktiviert
 *
 * @param i2ckeys Zustand der Taster T1-T4
 * @param zu_erf_abstand Der zu messende Abstand
 */
void Tkeyhandler(uint32_t *i2ckeys, uint32_t *zu_erf_abstand) {
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

/**
 * \brief <b>Ausgabe des LCD und der Front-LEDs je nach aktuell zu erfassendem Abstand.<br>
 * Außerdem wird hier die Reset-Funktion der Abstände ermöglicht.</b><br>
 * - Wenn L1 gerade gemessen wurde:
 *  - LED1 einschalten
 *  - Erste Zeile auf LCD schreiben -> "L1=" ABSTAND "cm"
 * - Wenn L2 gerade gemessen wurde:
 *  - LED2 einschalten
 *  - Zweite Zeile auf LCD schreiben -> "L2=" ABSTAND "cm. A=" FLÄCHENINHALT "qm"
 * - Wenn L3 gerade gemessen wurde:
 *  - LED3 einschalten
 *  - Dritte Zeile auf LCD schreiben -> "L3=" ABSTAND "cm. V=" VOLUMENINHALT "cbm"
 * - Wenn L4 übergeben wurde (T4 gedrückt):
 *  - Vierte Zeile auf LCD schreiben -> "Reset? T3: JA"
 *  - Nun auf Tastendruck des Users warten
 *   - Wenn T3 gedrückt:
 *    - Abstände L1-L3 im Array auf 0 setzen, sowie LCD-Inhalt löschen, Siebensegmentanzeige zurücksetzen und alle Front-LEDs ausschalten.
 *   - Wenn NICHT T3 gedrückt:
 *    - Zeile 4 des LCD löschen
 * - Wenn L1_1 gemessen wird (Alarmfunktion aktiv!):
 *  - Vierte Zeile auf LCD schreiben -> "ALRM" ALARMTYP "L1=" ABSTAND "cm"
 *
 * @param zustand Der gerade gemessende Abstand, L4 = RESET_DATA
 * @param abstand Distanz von dem gerade gemessenden Abstand
 */
void lcdhandler(uint32_t *zustand, uint32_t *abstand) {
	switch (*zustand) {
	case L1:
		writeFrontLED(I2CLED1);
		lcd_write_string_xy("L1=", 1, 1);
		lcd_write_uint(abstand[L1], 4);
		lcd_write_string_xy("cm", 8, 1);
		break;
	case L2:
		writeFrontLED(I2CLED1 & I2CLED2);
		lcd_write_string_xy("L2=", 1, 2);
		lcd_write_uint(abstand[L2], 4);
		lcd_write_string_xy("cm. A=", 8, 2);

		lcd_write_uint((abstand[L1] * abstand[L2]) / 10000, 5);
		lcd_write_string_xy("qm", 19, 2);
		break;
	case L3:
		writeFrontLED(I2CLED1 & I2CLED2 & I2CLED3);
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
			key = getTkeys();
		} while (key == 0b1111);         //Warte bis eine Eingabe erfolgte

		if (key == T3) {
			abstand[L1] = 0;
			abstand[L2] = 0;
			abstand[L3] = 0;
			lcd_clrscr();
			clrSegments();
			writeFrontLED(CLR_I2CLED);
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

/**
 * \brief <b>Funktion um Abstand zu speichern und diesen am Siebensegment und auf dem Display anzuzeigen. <br>
 *    Wenn T4 gedrückt wurde, wird diese Funktion nicht aufgerufen!</b><br>
 * - Abstand durch getDistance erhalten und im Array "abstande" an Position des zu erfassenden Abstandes speichern
 * - Abstand auf dem Siebensegment-Display anzeigen
 * - Ausgabe auf dem LCD
 * @param abstande Array beinhaltet alle gemessenden Abstände
 * @param zu_erf_abstand Der geforderte Abstand, der gemessen werden soll
 */
void saveDistance(uint32_t *abstande, uint32_t *zu_erf_abstand) {
	abstande[*zu_erf_abstand] = getDistance(); //Rückgabewert ist die Entfernung mit Gehäusetiefe
	setinttoSegment(abstande[*zu_erf_abstand]); //Jetzt auf das Siebensegment mappen
	lcdhandler(zu_erf_abstand, abstande);
}

/**
 * \brief <b>Erzeuge ein Tonsignal</b><br>
 * Ein Tonsignal wird über den auf dem Mainboard befindlichem Lautsprecher ausgegeben.<br>
 * Einstellungen sind in der main.h zu finden.
 */
void writeSpeaker() {
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

/**
 * \brief <b>Handler um bei eingeschaltetem Alarmzustand entsprechende Alarmfunktionen auszuführen</b><br>
 * - Wenn Alarmtyp unterschreiten:
 *  - Ermittlung, ob der aktuelle Abstand unter dem Abstand L1 liegt
 *   - Wenn ja, werden die roten LEDs auf dem Mainboard ein- und nach einer Zeit wieder ausgeschaltet. Zudem wird ein Tonsignal erzeugt.
 * - Wenn Alarmtyp überschreiten:
 *  - Ermittlung, ob der aktuelle Abstand über dem Abstand L1 liegt
 *   - Wenn ja, werden die roten LEDs auf dem Mainboard ein- und nach einer Zeit wieder ausgeschaltet. Zudem wird ein Tonsignal erzeugt.
 *
 * @param abstande Array beinhaltet alle gemessenden Abstände
 * @param alarmtyp Alarmtyp, der vom User gewählt wurde
 */
void distanzalarmHandler(uint32_t *abstande, uint32_t *alarmtyp) {
	switch (*alarmtyp) {
	case UNTER:  //TA1 wurde gedrueckt
		if (abstande[L1_1] < abstande[L1]) {
			writei2cRedLED(i2cRoteLEDsAN);
			delay(200);
			writei2cRedLED(i2cRoteLEDsAUS);
			writeSpeaker();
		}
		break;
	case UEBER: //TA2 wurde gedrueckt
		if (abstande[L1_1] > abstande[L1]) {
			writei2cRedLED(i2cRoteLEDsAN);
			delay(200);
			writei2cRedLED(i2cRoteLEDsAUS);
			writeSpeaker();
		}
		break;
	default:
		break;
	}

}

/**
 * \brief <b>Hauptfunktion, welches Programmabarbeitung je nach Zustandsänderung verzweigt.</b><br>
 * - I2C Verbindung intialisieren und auf Standard-Mode setzen (0.1 Mbit/s).
 * - LCD intialisieren und auf Standard-Mode setzen (0.1 Mbit/s). Sowie Cursor auf Modus 2 setzen -> Unterstrich
 * - Rote I2C LEDs auf dem Mainboard ausschalten
 * - GPIO-Ports intialisieren
 * - Jedes Element der Siebensegmentanzeige ausschalten
 * - RGB-LED ausschalten
 * - RIT intialisieren
 *
 * <b>Nun erfolgt eine Dauerschleife:</b>
 * - Taster T1 bis T4, sowie Taster TA1-TA3 auslesen
 *  - Wenn ein Taster von T1 bis T4 gedrückt wird
 *   - Handler für T-Keys aufrufen
 *   - saveDistance aufrufen
 *   - Wenn T4 gedrückt wurde (RESET_DATA), dann nur den lcdhandler aufrufen, da hier keine Distanz gemessen werden soll!
 *  - Wenn ein Taster von TA1 bis TA3 gedrückt wird oder wenn der Alarmzustand schon den Zustand HIGH eingenommen hat
 *   - Handler für TA-Keys aufrufen
 *   - Wenn Alarmzustand HIGH
 *    - Der zu erfassende Abstand wird auf L1_1 gesetzt
 *    - Abarbeitung von saveDistance
 *    - distanzalarmHandler wird aufgerufen
 * @return
 */
int main(void) {
	uint32_t clock = SystemCoreClock; //Um CCLK zu bekommen..

	uint32_t zu_erf_abstand = L1, i2ckeys = 0b1111, zi2ckeys = 0b1111, TAkeys =
			0b000, zTAkeys = 0b000, alrm_on = LOW, alrm_typ = UNTER;
	uint32_t abstande[5] = { 0, 0, 0, 0, 0 };

	i2c_init(I2C_SM);
	lcd_init(4, I2C_SM);
	lcd_cursor(2);
	writei2cRedLED(i2cRoteLEDsAUS);
	io_init(); //I/O-Ports initialisieren
	clrSegments();
	writeFrontLED(CLR_I2CLED);
	digitalWrite(RGB_R, RGB_Rport, RGB_AUS);
	digitalWrite(RGB_G, RGB_GBport, RGB_AUS);
	digitalWrite(RGB_B, RGB_GBport, RGB_AUS);
	timer_init();

	while (1) {
		i2ckeys = getTkeys();
		TAkeys = getTAkeys();

		if ((i2ckeys != zi2ckeys) && (i2ckeys != 0b1111)) {
			Tkeyhandler(&i2ckeys, &zu_erf_abstand);

			if ((zu_erf_abstand == L1)
					|| ((zu_erf_abstand == L2) && (abstande[L1] != 0))
					|| ((zu_erf_abstand == L3) && (abstande[L2] != 0))) {
				saveDistance(abstande, &zu_erf_abstand);
			}

			if (zu_erf_abstand == RESET_DATA)
				lcdhandler(&zu_erf_abstand, abstande);
		}

		if (((TAkeys != zTAkeys) && (TAkeys != 0b000) && (abstande[L1] != 0))
				|| alrm_on == HIGH) {
			TAkeyhandler(&TAkeys, &alrm_on, &alrm_typ);
			if (alrm_on == HIGH) {
				zu_erf_abstand = L1_1;
				saveDistance(abstande, &zu_erf_abstand);
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
