#ifndef EEPROM_manejo_h
#define EEPROM_manejo_h
#include <EEPROM.h>
#include "Declaraciones.h"

// Variables y funciones de la EEPROM
bool		EEPROM_programada;			// 0.	para verificar si está programada o no la EEPROM
float		temp_maxima_alarma;			// 1.
float		temp_minima_alarma;			// 2.
float		temp_maxima_ventilacion;	// 3.
uint8_t		humedad_suelo_minima;		// 4.	70 % es vaso de agua, 29 % es el aire
uint16_t	lapso_alarma_minutos;		// 5.	60 minutos (máx 65535 min o 1092 horas o 45 días)
bool		alarma_activada;			// 6.
uint16_t	tiempo_bombeo_segundos;		// 7.	4 segundos (máx 65535 seg o 18,2 horas)
uint16_t	tiempo_espera_minutos;		// 8.	15 minutos (máx 65535 min)

#define CANT_VARIABLES_EEPROM 9	// bool, float, float, float, int, int, bool, int, int
int longitud_dato_EEPROM[CANT_VARIABLES_EEPROM] = {1, 4, 4, 4, 1, 2, 1, 2, 2};
int direccion[CANT_VARIABLES_EEPROM];
int ESPACIOS_EEPROM;

void setDireccionesEEPROM() // en "setup()"
{
	// hay que hacer EEPROM.put a las direcciones indicadas, según la longitud de los datos escritos anteriormente
	int i;
	direccion[0] = 0;
	for (i = 1; i < CANT_VARIABLES_EEPROM; i++)
		direccion[i] = direccion[i - 1] + longitud_dato_EEPROM[i - 1];
	ESPACIOS_EEPROM = direccion[i - 1] + longitud_dato_EEPROM[i - 1];
}

//==================================================================================================================//

void chequearEEPROMProgramada() // en "setup()"
{
	setDireccionesEEPROM();
	EEPROM.begin(ESPACIOS_EEPROM); // la abrimos
	EEPROM_programada = (EEPROM.read(0) == 255 || EEPROM.read(0) == 0) ? false : true;
	EEPROM.end(); // la cerramos

	if (EEPROM_programada)
	{
		imprimirln("Hay que leer la EEPROM");
		leerEEPROMProgramada();
	}
	else
	{
		imprimirln("Hay que escribir la EEPROM");
		cargarValoresPorDefecto();
	}

#ifdef DEBUGserial
	imprimirEEPROMValsDirsReads();
#endif
}

//==================================================================================================================//

void leerEEPROMProgramada() // en "setup()"
{
	EEPROM.begin(ESPACIOS_EEPROM);
	EEPROM.get(direccion[1], temp_maxima_alarma);
	EEPROM.get(direccion[2], temp_minima_alarma);
	EEPROM.get(direccion[3], temp_maxima_ventilacion);
	EEPROM.get(direccion[4], humedad_suelo_minima);
	EEPROM.get(direccion[5], lapso_alarma_minutos);
	EEPROM.get(direccion[6], alarma_activada);
	EEPROM.get(direccion[7], tiempo_bombeo_segundos);
	EEPROM.get(direccion[8], tiempo_espera_minutos);
	EEPROM.end();
}

//==================================================================================================================//

// poner los valores por defecto en la EEPROM
void cargarValoresPorDefecto() // en "setup()"
{
	EEPROM_programada = true; // ahora va a estar programada
	temp_maxima_alarma = TEMP_MAXIMA_ALARMA_DEFECTO;
	temp_minima_alarma = TEMP_MINIMA_ALARMA_DEFECTO;
	temp_maxima_ventilacion = TEMP_MAXIMA_VENTILACION_DEFECTO;
	humedad_suelo_minima = HUMEDAD_SUELO_MINIMA_DEFECTO;
	lapso_alarma_minutos = LAPSO_ALARMA_MINUTOS_DEFECTO;
	alarma_activada = ALARMA_ACTIVADA_DEFECTO;
	tiempo_bombeo_segundos = TIEMPO_BOMBEO_SEGUNDOS_DEFECTO;
	tiempo_espera_minutos = TIEMPO_ESPERA_MINUTOS_DEFECTO;

	EEPROM.begin(ESPACIOS_EEPROM);
	EEPROM.put(direccion[0], EEPROM_programada);
	EEPROM.put(direccion[1], temp_maxima_alarma);
	EEPROM.put(direccion[2], temp_minima_alarma);
	EEPROM.put(direccion[3], temp_maxima_ventilacion);
	EEPROM.put(direccion[4], humedad_suelo_minima);
	EEPROM.put(direccion[5], lapso_alarma_minutos);
	EEPROM.put(direccion[6], alarma_activada);
	EEPROM.put(direccion[7], tiempo_bombeo_segundos);
	EEPROM.put(direccion[8], tiempo_espera_minutos);
	EEPROM.commit(); // efectivamente escribimos los valores
	EEPROM.end();
}

//==================================================================================================================//

template <typename T> // puede aceptar cualquier tipo de dato de entrada
void escribirEEPROM(int Adireccion, T Adato) // en chequearMensajesRecibidosTelegram()
{
	EEPROM.begin(ESPACIOS_EEPROM);
	EEPROM.put(Adireccion, Adato);
	EEPROM.commit();
	EEPROM.end();
}

//==================================================================================================================//

/*template <typename T> // puede devolver cualquier tipo de dato
T leerEEPROM(int Adireccion, T Adato) // en chequearMensajesRecibidosTelegram()
{
	EEPROM.begin(ESPACIOS_EEPROM);
	EEPROM.get(Adireccion, Adato); // llamamos a EEPROM.get pasándole el tipo de dato con que llamamos a leerEEPROM()
	EEPROM.end();
	return Adato;
}*/

//==================================================================================================================//

void imprimirEEPROMValsDirsReads()
{
	Serial.println();
	Serial.println("Valores recuperados de la EEPROM:");
	Serial.println(EEPROM_programada);
	Serial.println(temp_maxima_alarma);
	Serial.println(temp_minima_alarma);
	Serial.println(temp_maxima_ventilacion);
	Serial.println(humedad_suelo_minima);
	Serial.println(lapso_alarma_minutos);
	Serial.println(alarma_activada);
	Serial.println(tiempo_bombeo_segundos);
	Serial.println(tiempo_espera_minutos);
	Serial.println();

	Serial.println("Valores de las direcciones:");
	for (int i = 0; i < CANT_VARIABLES_EEPROM; i++)
	{
		Serial.print("Dir ");
		Serial.print(i);
		Serial.print(": ");
		Serial.println(direccion[i]);
	}
	Serial.println();

	EEPROM.begin(30);
	for (int j = 0; j < 30; j++)
	{
		Serial.print("Dirección ");
		Serial.print(j);
		Serial.print(": ");
		Serial.println(EEPROM.read(j));
	}
	EEPROM.end();
}

//=================================== ANTERIOR SISTEMA PARA setDireccionesEEPROM ===================================
/*
acá todas las variables solamente y después:

#define CANT_VARIABLES_EEPROM 10
int direccion[CANT_VARIABLES_EEPROM];
int ESPACIOS_EEPROM;

void setDireccionesEEPROM()
{
	// hay que hacer .put a las direcciones indicadas, según la longitud de los datos escritos anteriormente
	direccion[0] = 0;
	direccion[1] = direccion[0] + 1;//bool EEPROM_programada
	direccion[2] = direccion[1] + 4;//float	temp_maxima_alarma
	direccion[3] = direccion[2] + 4;//float	temp_minima_alarma
	direccion[5] = direccion[3] + 4;//float	temp_maxima_ventilacion
	direccion[6] = direccion[4] + 2;//int	humedad_suelo_minima
	direccion[7] = direccion[5] + 2;//int	lapso_alarma_minutos
	direccion[8] = direccion[6] + 1;//bool	alarma_activada
	direccion[9] = direccion[7] + 2;//int	tiempo_bombeo_segundos
 ESPACIOS_EEPROM = direccion[8] + 2;//int	tiempo_espera_minutos
}
*/


#endif