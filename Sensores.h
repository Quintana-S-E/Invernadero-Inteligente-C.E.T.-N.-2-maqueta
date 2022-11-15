#ifndef Sensores_h
#define Sensores_h
#include "Declaraciones.h"

void leerSensores() // en "loop()"
{
	// setea   humedad_aire_interior_promedio  y  temp_interior_promedio
	leerDHT22Interiores();
	// setea   humedad_suelo_interior
	leerSoilInteriores();

	// setea   humedad_aire_exterior  y  temp_exterior
	leerDHT22Exteriores();
	// setea   humedad_suelo_exterior
	leerSoilExteriores();
}

//==================================================================================================================//

void leerDHT22Interiores() // en leerSensores()
{
	// cada lectura toma alrededor de 250 ms, y puede tener hasta 2 segundos de antigüedad
	float humedad_aire_interior[2];
	float temp_interior[2];

	// leer a dhtInteriorLow
	humedad_aire_interior[0] = dhtInteriorLow.readHumidity(); // %
	temp_interior[0] = dhtInteriorLow.readTemperature();	  // Celsius

	/* leer a dhtInteriorMid
	humedad_aire_interior[0] = dhtInteriorMid.readHumidity();
	temp_interior[0] = dhtInteriorMid.readTemperature();
	*/

	// leer a dhtInteriorHigh
	humedad_aire_interior[1] = dhtInteriorHigh.readHumidity();
	temp_interior[1] = dhtInteriorHigh.readTemperature();

	// mirar si alguna lectura falló y finalizar antes (para empezar de nuevo).
	/*
	if (isnan(humedad_aire_interior[0]) || isnan(temp_interior[0])) {
		imprimirln("Fallo al leer el DHT22 interior bajo");
	}
	if (isnan(humedad_aire_interior[1]) || isnan(temp_interior[1])) {
		imprimirln("Fallo al leer el DHT22 interior alto");
	}*/

	humedad_aire_interior_promedio = (humedad_aire_interior[0] + humedad_aire_interior[1]) / 2;
	temp_interior_promedio = (temp_interior[0] + temp_interior[1]) / 2;
}

//==================================================================================================================//

void leerSoilInteriores() // en leerSensores()
{
	int humedad_suelo_interior_raw[MUESTRAS_HUMEDAD_SUELO];
	unsigned int humedad_suelo_interior_sum = 0;
	for (int i = 0; i < MUESTRAS_HUMEDAD_SUELO; i++)
	{
		humedad_suelo_interior_raw[i] = analogRead(SOIL_INT_PIN);
		humedad_suelo_interior_sum += humedad_suelo_interior_raw[i];
	}
	humedad_suelo_interior = (humedad_suelo_interior_sum / MUESTRAS_HUMEDAD_SUELO);
	// humedad_suelo_interior_perc = map(humedad_suelo_interior, 0, 4095, 0, 100);
}

//==================================================================================================================//

void leerDHT22Exteriores() // en leerSensores()
{
	// leer a dhtExterior
	humedad_aire_exterior = dhtExterior.readHumidity(); // %
	temp_exterior = dhtExterior.readTemperature();		// Celsius

	// mirar si alguna lectura falló y finalizar antes (para empezar de nuevo).
	/*
	if (isnan(humedad_aire_exterior) || isnan(temp_exterior)) {
		imprimirln("Fallo al leer el DHT22 exterior");
	}*/
	
	// con más sensores exteriores aquí haríamos el promedio
}

//==================================================================================================================//

void leerSoilExteriores() // en leerSensores()
{
	int humedad_suelo_exterior_raw[MUESTRAS_HUMEDAD_SUELO];
	unsigned int humedad_suelo_exterior_sum = 0;
	for (int i = 0; i < MUESTRAS_HUMEDAD_SUELO; i++)
	{
		humedad_suelo_exterior_raw[i] = analogRead(SOIL_EXT_PIN);
		humedad_suelo_exterior_sum += humedad_suelo_exterior_raw[i];
	}
	humedad_suelo_exterior = (humedad_suelo_exterior_sum / MUESTRAS_HUMEDAD_SUELO);
	// humedad_suelo_exterior = map(humedad_suelo_exterior, 0, 4095, 0, 100);
}


#endif