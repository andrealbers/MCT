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
 *  <b>Rote LEDs am Frontpanel</b>: Wenn eingeschaltet: Jeweiliger Abstand wurde bereits gemessen. <br>
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
 *	<b>lcdhandler:</b> Ausgabe auf das LC-Display und setzen der Front-LEDs je nach den gemessenden Abstandes. <br>
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
 ** <b>set_rgb:</b> Setze die zu übergebende RGB, welche auf dem Mainboard installiert ist. RGB_AUS schaltet alle 3 LEDs aus. <br>
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
#include "Siebensegment_TM1637.h"
#include "PCA9539.h"
#include "PCF8574.h"
#include "main.h"
#include "lcdlib_1769.h"
#include "i2c_1769.h"
#include "timer.h"

/**
 * \brief <b>Konfiguration der GPIO-Pins des Mikrocontrollers </b>
 */
void io_init(void) {   //PIN, PORT; MODE (PULLUP, PULLDOWN, OUTPUT)
	//Setze Taster TA1 bis TA3 als Eingänge im Pullup-Netzwerk
	pinMode(TA1pin, TAport, PULLUP);
	pinMode(TA2pin, TAport, PULLUP);
	pinMode(TA3pin, TAport, PULLUP);

	//Setze den Echopin des Ultraschallsensors bzw. des Treiber als Eingang im Pullup-Netzwerk
	pinMode(HC_Echopin, HCport, PULLUP);

	//Interrupt Ausgang des PCF8574 als Pullup setzen. Ein Interrupt liegt vor (LOW), wenn sich Flanken der inputs des ports ändern)
	pinMode(InterruptI2CFrontpin, InterruptI2CFrontport, PULLUP);

	//Orange LEDs auf dem Mainboard als Ausgang setzen
	pinMode(LED0pin, yLEDport, OUTPUT);  //Wird gar nicht gebraucht..
	pinMode(LED1pin, yLEDport, OUTPUT);
	pinMode(LED2pin, yLEDport, OUTPUT);
	pinMode(LED3pin, yLEDport, OUTPUT);
	pinMode(LED4pin, yLEDport, OUTPUT);
	pinMode(LED5pin, yLEDport, OUTPUT);
	pinMode(LED6pin, yLEDport, OUTPUT);
	pinMode(LED7pin, yLEDport, OUTPUT);

	//Triggerpin des Ultraschallsensors als Ausgang senden. Wenn positiver Impuls erfolgt, sendet dieser einen Burst aus.
	pinMode(HC_Triggerpin, HCport, OUTPUT);

	//Clock- und Datenleitung des TM1637 (Siebensegment) als Ausgang setzen
	pinMode(Siebensegment_CLKpin, Siebensegmentport, OUTPUT);
	pinMode(Siebensegment_DIOpin, Siebensegmentport, OUTPUT);

	//Alle 3 auf dem Board befindliche LEDs (R,G,B) als Ausgang schalten
	pinMode(RGB_Rpin, RGB_Rport, OUTPUT);
	pinMode(RGB_Gpin, RGB_GBport, OUTPUT);
	pinMode(RGB_Bpin, RGB_GBport, OUTPUT);

	//Piezo-Lautsprecher als Ausgang setzen
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
uint32_t getTAkeys() {                                   //Wenn Taster gedrückt -> digitalRead = LOW
	uint32_t TAzustand = 0b000;
	TAzustand |= ((!digitalRead(TA1pin, TAport)) << 0);  //LSB als Zustand für TA1
	TAzustand |= ((!digitalRead(TA2pin, TAport)) << 1);  //Zweites Bit als Zustand für TA2
	TAzustand |= ((!digitalRead(TA3pin, TAport)) << 2);  //MSB als Zustand für TA3
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
	case TA1:                                 //Wenn TA1 gedrückt wurde
		*alrm_on = HIGH;                      //Alarm-Zustand auf einschalten setzen
		*alrm_typ = UNTER;                    //Alarmtyp auf unterschreiten setzen
		lcd_write_string_xy("unter", 6, 4);   //Schreibe "unter" in letzte Zeile des Displays (Anzeige Alarmtyp)
		set_rgb(RGB_R | RGB_G);               //Rote und Grüne LED einschalten -> Gelb
		break;
	case TA2:                                 //Wenn TA2 gedrückt wurde
		*alrm_on = HIGH;                      //Alarm-Zustand auf einschalten setzen
		*alrm_typ = UEBER;                    //Alarmtyp auf überschreiten setzen
		lcd_write_string_xy("ueber", 6, 4);   //Schreibe "über" in letzte Zeile des Displays (Anzeige Alarmtyp)
		set_rgb(RGB_G);                       //Grüne LED einschalten
		break;
	case TA3:                                 //Wenn TA3 gedrückt wurde
		*alrm_on = LOW;                       //Alarm-Zustand auf ausschalten setzen
		set_rgb(RGB_AUS);                     //Alle 3 (Rot,Grün,Blau) RGB-LEDs ausschalten
		lcd_write_string_xy(Leerzeile, 1, 4); //Letzte Zeile von LC-Display löschen
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
	case T1:                             //Wenn T1 gedrückt wurde
		*zu_erf_abstand = L1;            //Setze den nun zu erfassenden Abstand auf L1
		break;
	case T2:                             //Wenn T1 gedrückt wurde
		*zu_erf_abstand = L2;            //Setze den nun zu erfassenden Abstand auf L2
		break;
	case T3:                             //Wenn T1 gedrückt wurde
		*zu_erf_abstand = L3;            //Setze den nun zu erfassenden Abstand auf L3
		break;
	case T4:                             //Wenn T1 gedrückt wurde
		*zu_erf_abstand = RESET_DATA;    //Setze den nun zu erfassenden Abstand auf RESET_DATA (alle gemessenen Abstände löschen)
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
 * @param ZustandI2CLEDs Zustand der Frontpanel-LEDs
 */
void lcdhandler(uint32_t *zustand, uint32_t *abstand, uint32_t *ZustandI2CLEDs) {

	switch (*zustand) {
	case L1:                                   //Wenn der zu erfassende Abstand L1 ist
		lcd_write_string_xy(Leerzeile, 1, 1);  //Erste Zeile des LC-Displays löschen
		*ZustandI2CLEDs &= I2CLED1;            //Bit für Frontpanel LED1 setzen (einschalten)
		lcd_write_string_xy("L1=", 1, 1);
		lcd_write_uint(abstand[L1], 4);
		lcd_write_string_xy("cm", 8, 1);
		//	break;                             //Kein break, da nun auch der Flächeninhalt aktualisiert werden soll
	case L2:
		if (abstand[L2] != 0) {                      //Nur die zweite Zeile beschreiben, wenn vorher ein Wert ermittelt wurde
			lcd_write_string_xy(Leerzeile, 1, 2);
			*ZustandI2CLEDs &= I2CLED2;              //Bit für Frontpanel LED2 setzen (einschalten)
			lcd_write_string_xy("L2=", 1, 2);
			lcd_write_uint(abstand[L2], 4);
			lcd_write_string_xy("cm, A=", 8, 2);
			uint32_t flaecheninhaltcm = abstand[L1] * abstand[L2],
					flaecheninhaltqm = flaecheninhaltcm / 10000,
					_flaecheninhaltcm = flaecheninhaltcm, _flaecheninhaltqm =
							flaecheninhaltqm, anzStellencm = 0,
					anzStellenqm = 0;

			while (_flaecheninhaltcm != 0) {                 //Erhalte Anzahl der ganzen Stellen von dem Flächeninhalt in cm2
				_flaecheninhaltcm = _flaecheninhaltcm / 10;
				anzStellencm++;
			}

			while (_flaecheninhaltqm != 0) {                 //Erhalte Anzahl der ganzen Stellen von dem Flächeninhalt in m2
				_flaecheninhaltqm = _flaecheninhaltqm / 10;
				anzStellenqm++;
			}

			if (flaecheninhaltqm == 0) {                            //Wenn Flächeninhalt <1qm
				uint32_t _anzStellencm = anzStellencm, stelle = 0;
				lcd_write_string_xy("0.", 14, 2);
				while (_anzStellencm < 4) {                         //Fülle Nullen nach dem Komma, bis insgesamt 3 Stellen nach dem Komma dargestellt werden -> 123cm2 -> 0.012m2
					lcd_write_string_xy("0", 16 + stelle, 2);       //16 ist die erste Stelle nach dem "0."
					stelle++;
					_anzStellencm++;
				}                                                   //Anzahl der Stellen-2 entspricht immer der Anzahl der darzustellenden Zahlen
				lcd_write_uint(flaecheninhaltcm, anzStellencm - 2); //1234cm2(4 Stellen) -> 0,12m2(2 Stellen) bzw. 123cm2(3 Stellen) -> 0,01m2(1 Stelle)

			} else {                                                //Wenn Flächeninhalt >1qm
				lcd_write_uint(flaecheninhaltqm, anzStellenqm);     //Schreibe erste Zahlen vom Flächeninhalt in m2
				if (anzStellenqm < 3) {
					lcd_write_string_xy(".", 14 + anzStellenqm, 2); //Setze "." nach den ganzen Zahlen

					while (anzStellencm > 3) {                      //Teile so lange den Flächeninhalt (cm2) durch 10, bis die Zahl nur noch aus 3 Ziffern besteht
						flaecheninhaltcm = flaecheninhaltcm / 10;
						anzStellencm--;
					}

					uint32_t i = 0;
					while (anzStellenqm < 3) {                     //3 Stellen darstellen (4 mit Punkt)
						lcd_gotoxy(17 - i, 2);                     //Setze Cursor auf die letzte zu beschreibende Position für Ziffer
						lcd_write_uint(flaecheninhaltcm % 10, 1);  //Schreibe letzte Ziffer auf Display
						flaecheninhaltcm = flaecheninhaltcm / 10;  //Teile durch 10, um nächste Ziffer zu erhalten
						anzStellenqm++;
						i++;
					}
				}
			}
			lcd_write_string_xy("qm", 19, 2);
		}
		//break;                              //Kein break, da nun auch das Volumen aktualisiert werden soll
	case L3:
		if (abstand[L3] != 0) {                              //analog zu L2
			lcd_write_string_xy(Leerzeile, 1, 3);
			*ZustandI2CLEDs &= I2CLED3;
			lcd_write_string_xy("L3=", 1, 3);
			lcd_write_uint(abstand[L3], 4);
			lcd_write_string_xy("cm, V=", 8, 3);
			uint32_t volumenccm = abstand[L1] * abstand[L2] * abstand[L3],
					volumencbm = volumenccm / 1000000, _volumenccm = volumenccm,
					_volumencbm = volumencbm, anzStellenccm = 0, anzStellencbm =
							0;

			while (_volumenccm != 0) {            //Erhalte Anzahl der ganzen Stellen von dem Volumen in cm3
				_volumenccm = _volumenccm / 10;
				anzStellenccm++;
			}

			while (_volumencbm != 0) {            //Erhalte Anzahl der ganzen Stellen von dem Volumen in m3
				_volumencbm = _volumencbm / 10;
				anzStellencbm++;
			}

			if (volumencbm == 0) {                //Wenn Volumen <1m3
				uint32_t _anzStellenccm = anzStellenccm, stelle = 0;
				lcd_write_string_xy("0.", 14, 3);
				while (_anzStellenccm < 6) {
					lcd_write_string_xy("0", 16 + stelle, 3);
					stelle++;
					_anzStellenccm++;
				}
				lcd_write_uint(volumenccm / 1000, anzStellenccm - 4);
			} else {                                                     //Wenn Volumen >1m3
				lcd_write_uint(volumencbm, anzStellencbm);               //Schreibe erste Zahlen vom Volumen in m3
				if (anzStellencbm < 3) {
					lcd_write_string_xy(".", 14 + anzStellencbm, 3);     //Setze "." nach den ganzen Zahlen

					while (anzStellenccm > 3) {                          //Teile so lange die Zahl durch 10, bis 3 Ziffern erhalten
						volumenccm = volumenccm / 10;
						anzStellenccm--;
					}

					uint32_t i = 0;
					while (anzStellencbm < 3) {
						lcd_gotoxy(17 - i, 3);                //Setze Cursor auf die letzte zu beschreibende Position für Ziffer
						lcd_write_uint(volumenccm % 10, 1);   //Schreibe letzte Ziffer auf Display
						volumenccm = volumenccm / 10;         //Teile durch 10, um nächste Ziffer zu erhalten
						anzStellencbm++;
						i++;
					}
				}
			}
			lcd_write_string_xy("cbm", 18, 3);
		}
		break;
	case RESET_DATA:
		lcd_write_string_xy("Reset? T3: JA", 1, 4);
		uint32_t key = 0;

		do {
			delay(10);
			key = getTkeys();
		} while (key == 0b1111);         //Warte bis eine Eingabe erfolgte

		if (key == T3) {                 //Wenn T3 gedrückt wurde
			abstand[L1] = 0;             //Lösche alle gespeicherten Abstände
			abstand[L2] = 0;
			abstand[L3] = 0;
			lcd_clrscr();                //Lösche LC-Display Anzeige
			clrSegments();               //Lösche alle Segmente der Siebensegmentanzeige
			*ZustandI2CLEDs = CLR_I2CLED;//Lösche Frontpanel-LEDs
		}

		lcd_write_string_xy(Leerzeile, 1, 4); //Lösche letzte Zeile des LC-Displays
		break;
	case L1_1:                               //Alarmzustand AKTIV
		lcd_write_string_xy("ALRM:", 1, 4);  //Alarmtyp wird von "TAkeyhandler" geschrieben
		lcd_write_string_xy("L1=", 12, 4);
		lcd_write_uint(abstand[L1_1], 4);
		lcd_write_string_xy("cm", 19, 4);
		break;
	default:
		break;
	}
	writeFrontLED(*ZustandI2CLEDs);     //Setze Frontpanel-LEDs je nach Zustand
}
/**
 * \brief <b>Funktion um Abstand zu speichern und diesen am Siebensegment und auf dem Display anzuzeigen. <br>
 *    Wenn T4 gedrückt wurde, wird diese Funktion nicht aufgerufen!</b><br>
 * - Abstand durch getDistance erhalten und im Array "abstande" an Position des zu erfassenden Abstandes speichern
 * - Abstand auf dem Siebensegment-Display anzeigen
 * - Ausgabe auf dem LCD
 * @param abstande Array beinhaltet alle gemessenden Abstände
 * @param zu_erf_abstand Der geforderte Abstand, der gemessen werden soll
 * @param ZustandFrontLEDs Zustand der Frontpanel-LEDs
 */
void saveDistance(uint32_t *abstande, uint32_t *zu_erf_abstand, uint32_t *ZustandFrontLEDs) {
	abstande[*zu_erf_abstand] = getDistance();                //Rückgabewert ist die Entfernung mit Gehäusetiefe
	setinttoSegment(abstande[*zu_erf_abstand]);               //Jetzt auf das Siebensegment mappen
	lcdhandler(zu_erf_abstand, abstande, ZustandFrontLEDs);   //Nun LC-Display beschreiben
}

/**
 * \brief <b>Erzeuge ein Tonsignal</b><br>
 * Ein Tonsignal wird über den auf dem Mainboard befindlichem Lautsprecher ausgegeben.<br>
 * Einstellungen sind in der main.h zu finden.
 */
void writeSpeaker() {
	for (uint32_t i = startwert; i < abtastzeit; i++) {    //._.__.___.____.____.___.__._ erzeugen (_ entspricht Einschaltdauer)
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
void distanzalarmHandler(uint32_t *abstande, uint32_t *alarmtyp) {  //Hier möglich eine Variable zu erstellen, die bei erfüllten if Abfragen gesetzt wird..
	switch (*alarmtyp) {
	case UNTER:                                //TA1 wurde gedrueckt
		if (abstande[L1_1] < abstande[L1]) {   //Wenn der aktuell gemessende Abstand kleiner ist als der gespeicherte Wert L1
			writei2cRedLED(i2cRoteLEDsAN);     //8 rote LEDs (von links) auf dem Mainboard einschalten
			delay(200);
			writei2cRedLED(i2cRoteLEDsAUS);    //8 rote LEDs (von links) auf dem Mainboard ausschalten
			writeSpeaker();                    //Tonsignal erzeugen
		}
		break;
	case UEBER:                                //TA2 wurde gedrueckt
		if (abstande[L1_1] > abstande[L1]) {   //Wenn der aktuell gemessende Abstand größer ist als der gespeicherte Wert L1
			writei2cRedLED(i2cRoteLEDsAN);     //8 rote LEDs (von links) auf dem Mainboard einschalten
			delay(200);
			writei2cRedLED(i2cRoteLEDsAUS);    //8 rote LEDs (von links) auf dem Mainboard ausschalten
			writeSpeaker();                    //Tonsignal erzeugen
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
	//uint32_t clock = SystemCoreClock; //Um CCLK zu bekommen..

	uint32_t zu_erf_abstand = L1, i2ckeys = Tnokey, zi2ckeys = Tnokey, TAkeys =
	TAnokey, zTAkeys = TAnokey, alrm_on = LOW, alrm_typ =
	UNTER, ZustandFrontLEDs = CLR_I2CLED;
	uint32_t abstande[5] = { 0, 0, 0, 0, 0 };

	i2c_init(I2C_SM);                //i2c initialisieren im Standard-Mode
	lcd_init(4, I2C_SM);             //LCD intialisieren im Standard-Mode
	lcd_cursor(2);                   //Cursor vom LC-Display auf Unterstrich setzen
	writei2cRedLED(i2cRoteLEDsAUS);  //Rote LEDs des Mainboards ausschalten
	io_init();                       //I/O-Ports initialisieren
	clrSegments();                   //Alle Segmente der Siebensegmente ausschalten
	writeFrontLED(CLR_I2CLED);       //Frontpanel-LEDs ausschalten
	set_rgb(RGB_AUS);                //Auf dem Board befindliche RGB-LED ausschalten
	timer_init();                    //RIT initialisieren

	while (1) {
		i2ckeys = getTkeys();  //Erhalte Zustand von T1-T4
		TAkeys = getTAkeys();  //Erhalte Zustand von TA1-TA3

		if ((i2ckeys != zi2ckeys) && (i2ckeys != Tnokey)) {  //Wenn Zustand von T1-T4 geändert
			Tkeyhandler(&i2ckeys, &zu_erf_abstand);

			if ((zu_erf_abstand == L1)                                    //Wenn L1 gemessen werden soll
					|| ((zu_erf_abstand == L2) && (abstande[L1] != 0))    //Wenn L2 gemessen werden soll und L1 schon gemessen wurde
					|| ((zu_erf_abstand == L3) && (abstande[L2] != 0))) { //Wenn L3 gemessen werden soll und L2 schon gemessen wurde
				saveDistance(abstande, &zu_erf_abstand, &ZustandFrontLEDs);
			}

			if (zu_erf_abstand == RESET_DATA)  //Wenn die gespeicherten Abstände gelöscht werden sollen, T4 gedrückt
				lcdhandler(&zu_erf_abstand, abstande, &ZustandFrontLEDs);
		}

		if (((TAkeys != zTAkeys) && (TAkeys != TAnokey) && (abstande[L1] != 0))   //Wenn Zustand von TA1-TA3 geändert, oder Alarmzustand aktiv
				|| alrm_on == HIGH) {
			TAkeyhandler(&TAkeys, &alrm_on, &alrm_typ);
			if (alrm_on == HIGH) {      //Wenn Alarmzustand aktiv
				zu_erf_abstand = L1_1;
				saveDistance(abstande, &zu_erf_abstand, &ZustandFrontLEDs);
				distanzalarmHandler(abstande, &alrm_typ);
			}
		}

		zi2ckeys = i2ckeys;  //Speichere letzten Zustand der Taster T1-T4
		zTAkeys = TAkeys;    //Speichere letzten Zustand der Taster TA1-TA3
		delay(10);
	}

	i2c_exit();
	return 0;
}
