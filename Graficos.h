#ifndef Graficos_h
#define Graficos_h
#include <ThingSpeak.h>
#include "Declaraciones.h"
#include "Claves.h"
/*Buscar mejor en la librería del BOT, pero o creamos acá un WifiClient o hacemos un WiFiClientSecure*/

void actualizarGraficos()
{
    ThingSpeak.setField(1, temp_interior_promedio);
    ThingSpeak.setField(2, temp_exterior);
    ThingSpeak.setField(3, humedad_aire_interior_promedio);
    ThingSpeak.setField(4, humedad_aire_exterior);
    ThingSpeak.setField(5, humedad_suelo_interior);
    ThingSpeak.setField(6, humedad_suelo_exterior);

    int x = ThingSpeak.writeFields(1, thingspeak_api_key);
    if (x != 200) imprimirln("Error actualizando ThingSpeak");
}


#endif