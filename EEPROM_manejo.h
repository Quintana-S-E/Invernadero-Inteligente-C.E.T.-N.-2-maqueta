#ifndef EEPROM_manejo_h
#define EEPROM_manejo_h
#include <EEPROM.h>
#include "Declaraciones.h"

// Variables y funciones de la EEPROM
bool EEPROM_programada;				// 0.	para verificar si está programada o no la EEPROM
float temp_max_alarma = 45.0F;		// 1.
float temp_min_alarma = -5.0F;	// 2.
float temp_max_ventilacion = 35.0F;	// 3.
uint16_t sequedad_suelo_max = 1200; // 4.
uint16_t lapso_alarma_mins = 60;	// 5.	60 minutos (máx 65535 min o 1092 horas o 45 días)
bool alarma_activada = true;		// 6.
uint16_t tiempo_de_bomba_segs = 10;	// 7.	4 segundos (máx 65535 seg o 18,2 horas)
uint16_t tiempo_espera_mins = 15;	// 8.	15 minutos (máx 65535 min)

#define CANT_VARIABLES_EEPROM 9	// bool, float, float, float, int, int, bool, int, int
int long_dato[CANT_VARIABLES_EEPROM] = {1, 4, 4, 4, 2, 2, 1, 2, 2};
int direccion[CANT_VARIABLES_EEPROM];
int ESPACIOS_EEPROM;

void setDireccionesEEPROM() // en "setup()"
{
	// hay que hacer EEPROM.put a las direcciones indicadas, según la longitud de los datos escritos anteriormente
	int i;
	direccion[0] = 0;
	for (i = 1; i < CANT_VARIABLES_EEPROM; i++) {
		direccion[i] = direccion[i - 1] + long_dato[i - 1];
	}
	ESPACIOS_EEPROM = direccion[i - 1] + long_dato[i - 1];
}

//==================================================================================================================//

void chequearEEPROMProgramada() // en "setup()"
{
	setDireccionesEEPROM();
	EEPROM.begin(ESPACIOS_EEPROM); // la abrimos
	EEPROM_programada = (EEPROM.read(0) == 255 || EEPROM.read(0) == 0) ? false : true;
	EEPROM.end(); // la cerramos

	if (EEPROM_programada) {
		imprimirln("Hay que leer la EEPROM");
		leerEEPROMProgramada();
	}
	else {
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
	EEPROM.get(direccion[1], temp_max_alarma);
	EEPROM.get(direccion[2], temp_min_alarma);
	EEPROM.get(direccion[3], temp_max_ventilacion);
	EEPROM.get(direccion[4], sequedad_suelo_max);
	EEPROM.get(direccion[5], lapso_alarma_mins);
	EEPROM.get(direccion[6], alarma_activada);
	EEPROM.get(direccion[7], tiempo_de_bomba_segs);
	EEPROM.get(direccion[8], tiempo_espera_mins);
	EEPROM.end();
}

//==================================================================================================================//

// poner los valores por defecto en la EEPROM
void cargarValoresPorDefecto() // en "setup()"
{
	EEPROM.begin(ESPACIOS_EEPROM);
	EEPROM_programada = true; // ahora va a estar programada
	EEPROM.put(direccion[0], EEPROM_programada);
	EEPROM.put(direccion[1], temp_max_alarma);
	EEPROM.put(direccion[2], temp_min_alarma);
	EEPROM.put(direccion[3], temp_max_ventilacion);
	EEPROM.put(direccion[4], sequedad_suelo_max);
	EEPROM.put(direccion[5], lapso_alarma_mins);
	EEPROM.put(direccion[6], alarma_activada);
	EEPROM.put(direccion[7], tiempo_de_bomba_segs);
	EEPROM.put(direccion[8], tiempo_espera_mins);
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
	Serial.println(temp_max_alarma);
	Serial.println(temp_min_alarma);
	Serial.println(temp_max_ventilacion);
	Serial.println(sequedad_suelo_max);
	Serial.println(lapso_alarma_mins);
	Serial.println(alarma_activada);
	Serial.println(tiempo_de_bomba_segs);
	Serial.println(tiempo_espera_mins);
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

void setDireccionesEEPROM(){
	// hay que hacer .put a las direcciones indicadas, según la longitud de los datos escritos anteriormente
	direccion[0] = 0;
	direccion[1] = direccion[0] + 1;//bool EEPROM_programada
	direccion[2] = direccion[1] + 4;//float	temp_max_alarma
	direccion[3] = direccion[2] + 4;//float	temp_min_alarma
	direccion[5] = direccion[3] + 4;//float	temp_max_ventilacion
	direccion[6] = direccion[4] + 2;//int	sequedad_suelo_max
	direccion[7] = direccion[5] + 2;//int	lapso_alarma_mins
	direccion[8] = direccion[6] + 1;//bool	alarma_activada
	direccion[9] = direccion[7] + 2;//int	tiempo_de_bomba_segs
 ESPACIOS_EEPROM = direccion[8] + 2;//int	tiempo_espera_mins
}
*/


#endif