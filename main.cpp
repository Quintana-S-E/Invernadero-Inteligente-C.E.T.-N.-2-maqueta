/*------------------------------------------------------------------------------------------------------------------*\
	Nombre:				Invernadero Inteligente maqueta ESP32 Node32s
	Desarrollo: 		octubre y noviembre de 2022
	Primer lanzamiento: 14 de noviembre de 2022
	Creado por:			Pulido Norberto N., Quintana Santiago E., Riha Fabio, Sacchero Fidel, Inticito.
\*------------------------------------------------------------------------------------------------------------------*/

#include "Control.h"
#include "Declaraciones.h" // contiene <Arduino.h> y todas las librerías
#include "Display.h"
#include "EEPROM_manejo.h"
#include "Sensores.h"
#include "Telegram.h"

#include "Claves.h"

void setup()
{
	// inicializaciones varias
	Serial.begin(9600);
	pinMode(LED_BUILTIN, OUTPUT);
	pinMode(LED_ROJO, OUTPUT);
	pinMode(PIN_BOMBA, OUTPUT);
	pinMode(PIN_VENTILADOR, OUTPUT);
	digitalWrite(PIN_BOMBA, HIGH);
	digitalWrite(PIN_VENTILADOR, HIGH);

	// inicializar servo
		ESP32PWM::allocateTimer(0);
		ESP32PWM::allocateTimer(1);
		ESP32PWM::allocateTimer(2);
		ESP32PWM::allocateTimer(3);
	ventana.setPeriodHertz(50);
	ventana.attach(PIN_SERVO);
	ventana.write(ANGULO_CERRADO); // ya que "ventilando" se inicializa como false

	// inicializar los sensores
	dhtInteriorLow.begin();
	dhtInteriorHigh.begin();
	dhtExterior.begin();

	// inicializar display
	inicializarDisplay();

	// conectarse al Wi-Fi
	imprimirln("Conectando a WIFI...");
	displayConectandoWIFI();

	bot.setMaxConnectionRetries(15);
	bool status_WIFI = false;
	if (!status_WIFI) status_WIFI = conectarWIFI(SSIDescuela, PASSWORDescuela); // conectar al WIFI de la escuela
	if (!status_WIFI) status_WIFI = conectarWIFI(SSIDnoni, PASSWORDnoni); // si no se pudo, probar con el de noni
	if (!status_WIFI) status_WIFI = conectarWIFI(SSIDsanti, PASSWORDsanti); // si no se pudo, probar con el de santi
	if (!status_WIFI)
	{
		imprimirln("No se encuentra red WIFI.");
		displayErrorWIFI();
	}

	// conectar el bot a Telegram
	bot.setTelegramToken(BOT_TOKEN);

	// leer o escribir la EEPROM
	chequearEEPROMProgramada();

	delay(5000);
	imprimirln("Setup finalizado!");
}

//=======================================aquí se encuentra la función loop()========================================//

void loop()
{
	if (millis() - ultima_vez_invernadero_funciono >= DELAY_ACTIVIDAD_INVERNADERO)
	{ // idealmente, en lugar de esperas pondríamos al uC en un estado de bajo consumo por un período fijo
		ultima_vez_invernadero_funciono = millis();

		// Leer sensores
		leerSensores();

		// Manejar Telegram
		chequearMensajesRecibidosTelegram();
		chequearAlarma();

		// Actualizar datos en el display
		displayLecturas(mostrando_humedad);

		// Tomar decisiones
		chequearVentilacion();
		chequearRiego();
		// chequear_iluminacion();

		digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN)); // parpadeamos el LED de la placa
	}

	// cambiamos el contenido de la pantalla (de mostrar humedad a temperatura y viceversa)
	if (millis() - ultima_vez_display_cambio >= DELAY_CAMBIO_DISPLAY)
	{
		ultima_vez_display_cambio = millis();
		mostrando_humedad = !mostrando_humedad;
	}
}