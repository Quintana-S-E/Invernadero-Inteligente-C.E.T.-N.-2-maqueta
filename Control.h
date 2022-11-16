#ifndef Control_h
#define Control_h
#include "Declaraciones.h"
#include "EEPROM_manejo.h"

// Abrir la ventilación y activar ventiladores
void chequearVentilacion() // en "loop()"
{
	if (ventilacion_forzada) return; // si la ventilación es forzada por telegram, ignorar lo automático

	if (temp_exterior > temp_max_ventilacion && !ventilando)
	{
		ventilando = true;
		acivarVentilacion();
	}
	else if (temp_exterior <= TEMP_CIERRE_VENTILACION && ventilando)
	{
		ventilando = false;
		desactivarVentilacion();
	}
}

//==================================================================================================================//

// Regar y esperar el tiempo necesario para la filtración del agua antes de medir de nuevo
void chequearRiego() // en "loop()"
{
	// apagar la bomba después del tiempo definido (ADV: VA A SER MEJOR PONER UN DELAY, CAPAZ)
	if (millis() - ultima_vez_bomba >= (tiempo_de_bomba_segs * 1000UL)){
		digitalWrite(PIN_BOMBA, HIGH);
	}

	// si se está esperando, comprobar si pasó el tiempo desde ultima_vez_bomba. De ser así, dejar de esperar
	if (esperando_riego)
	{
		if (millis() - ultima_vez_bomba >= (tiempo_espera_mins * 60000UL))
		{
			esperando_riego = false;
			imprimirln("La espera desde el riego finalizó");
		}
	}

	// chequear la humedad y regar (si no se está esperando la filtración del agua)
	if (humedad_suelo_exterior >= sequedad_suelo_max && !esperando_riego)
	{
		ultima_vez_bomba = millis();
		digitalWrite(PIN_BOMBA, LOW);
		esperando_riego = true; // hay que esperar desde el tiempo 0 (ultima_vez_bomba)
	}
}

//==================================================================================================================//

void acivarVentilacion()
{
	Ventana.write(ANGULO_APERTURA);
	digitalWrite(PIN_VENTILADOR, LOW);
}

//==================================================================================================================//

void desactivarVentilacion()
{
	Ventana.write(ANGULO_CERRADO);
	digitalWrite(PIN_VENTILADOR, HIGH);
}

//==================================================================================================================//

/*// Identifica la necesidad de iluminar, basándose en la lectura de un sensor LDR
void chequear_iluminacion()
{
	//...
}*/


#endif