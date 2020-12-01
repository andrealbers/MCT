/**
 *  \file PCF8574.c
 */

#include "PCF8574.h"

/**
 * \brief <b>LED1-LED4 ein/ausschalten</b>
 * - Verbindung zum Portexpander PCF8574 wird schreibend aufgebaut
 * - Anschließend wird ein Byte direkt in das Register geschrieben, um entsprechenden Ausgang zu setzen
 * @param led Die einzuschaltende LED (s. PCF8574.h)
 */
void writeFrontLED(uint32_t led) {
	i2c_start();
	i2c_write_byte(I2C_PCF8574A_ADDR | I2C_WRITE);
	i2c_write_byte(led);
	i2c_stop();
}

/**
 * \brief <b>Zustand der Taster T1-T4 erhalten</b><br>
 * - Verbindung zum Portexpander PCF8574 wird lesend aufgebaut, wenn sich der Tasterzustand geändert hat
 * - Der Rückgabewert von i2c_read_byte wird in i2ckeys gespeichert
 * - Verbindung wird gestoppt
 * - i2ckeys wird um 4 nach rechts geshiftet, um ersten Taster zum LSB zu machen (nicht unbedingt notwendig..)
 * - i2ckeys wird mit 0b1111 verundet, um nur den Zustand der Taster zu erhalten (wenn shr raus, hier auch 0b11110000 möglich..)
 * - Zustandsbit changed wieder zurücksetzen
 * @return Zustand der Taster T1-T4
 */
uint32_t getTkeys() {
	uint32_t i2ckeys = Tnokey;

	if (changed) {        //Wenn sich überhaupt ein Eingang des Expanders geändert hat
		i2c_start();
		i2c_write_byte(I2C_PCF8574A_ADDR | I2C_READ);
		i2ckeys = i2c_read_byte(0);  //Erhalte Zustand des Portexpander Registers
		i2c_stop();
		i2ckeys = i2ckeys >> 4;  //Mache T1 zum LSB (nächstes Bit T2, T3 und T4)
		i2ckeys &= (Tnokey);     //Nur Taster auslesen
		changed = 0;     //Zurücksetzen des Merkers, ob sich der Zustand Taster T1-T4 geändert hat
	}
	return i2ckeys; //0, wenn Taster gedrückt

}
