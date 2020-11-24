/**
 *  \file PCA9539.c
 */

#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif
#include <PCA9539.h>
#include "i2c_1769.h"

/**
 * \brief <b>Die 8 linken roten LEDs auf dem Mainboard ein/ausschalten</b><br>
 * - Wenn der Übergabewert i2cRoteLEDsAUS ist:
 *  - Die Verbindung zum PCA9539 wird über i2c_start schreibend gestartet
 *  - Configuration Port 0 wird adressiert
 *  - In diesem Register werden alle Bits 1 gesetzt, um diese als Eingang zu setzen
 *   - Dies ist notwendig, da die LEDs auch leuchten, wenn das Register auf Ausgang gesetzt und ausgeschaltet werden sollen, da diese dennoch sehr gedimmt leuchten.
 *  - Es werden zwei Bytes gesendet -> Werte im Register Configuration Port 1 werden auch gesetzt
 * - Sonst:
 *  - Die Verbindung zum PCA9539 wird schreibend gestartet
 *   - Configuration Port 1 wird adressiert (Hier sind die linken 8 roten LEDs angeschlossen)
 *   - In diesem Register werden alle Bits 0 gesetzt, um diese als Ausgang zu setzen
 *   - Verbindung wird gestoppt
 *  - Die Verbindung wird erneut schreibend aufgebaut
 *   - Output Port 1 wird adressiert (Hier sind die linken 8 roten LEDs angeschlossen)
 *   - In diesem Register werden alle Bits 0 gesetzt, um die LEDs einzuschalten
 * @param setleds Die 8 linken LEDs ein/ausschalten
 */
void writei2cRedLED(uint32_t setleds) {
	if (setleds == i2cRoteLEDsAUS) {
		//Pins des Port Expanders als Eingang setzen
		i2c_start();
		i2c_write_byte(I2C_PCA9539_ADDR | I2C_WRITE);
		i2c_write_byte(0x06);
		i2c_write_byte(0xff); //Pins werden als Eingang gesetzt, da die LEDs bei Aus noch leicht leuchten!
		i2c_write_byte(0xff);
	} else {
		//LEDs einschalten
		i2c_start();
		i2c_write_byte(I2C_PCA9539_ADDR | I2C_WRITE);
		i2c_write_byte(0x07);
		i2c_write_byte(0x00);
		i2c_stop();

		i2c_start();
		i2c_write_byte(I2C_PCA9539_ADDR | I2C_WRITE);
		i2c_write_byte(0x03);
		i2c_write_byte(0x00);
		i2c_stop();
	}

}
