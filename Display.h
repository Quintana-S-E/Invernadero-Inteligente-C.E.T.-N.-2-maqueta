#ifndef Display_h
#define Display_h
#include "Declaraciones.h"

void inicializarDisplay() // en "setup()"
{
	if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
	{
		imprimirln("Fallo en el display. Resetear el equipo.");
		while (1) ;
	}
	delay(2000);
	display.clearDisplay();
	display.setTextColor(WHITE);
}

//==================================================================================================================//

void displayConectandoWIFI() // en "setup()"
{
	display.setTextSize(2);
	display.setCursor(0, 0);
	display.print("Conectando\na WIFI...");
	display.display();
}

//==================================================================================================================//

void displayErrorWIFI() // en "setup()"
{
	display.clearDisplay();
	display.setTextSize(2);
	display.setCursor(0, 0);
	display.print("No se\nencuentra\nred WIFI.");
	display.display();
}

//==================================================================================================================//

void displayConexionWIFI(String Amensaje_conectado_a, String Assid_conectada) // en "conectarWIFI()"
{
	display.clearDisplay();
	display.setTextSize(2);				// en grande:
	display.setCursor(0, 0);
	display.print(Amensaje_conectado_a);// conectado a la red
	display.setTextSize(1);				// en chiquito:
	display.setCursor(0, 40);
	display.print(Assid_conectada);		// nombre de la red
	display.display();
}

//==================================================================================================================//

// sólo actualizamos los números del display cada cierto tiempo (numbers flicker)
void displayLecturas(bool Amostrando_humedad) // en "loop()"
{
	if (millis() - ultima_vez_display_actualizo >= DELAY_ACTUALIZACION_DISPLAY)
	{
		ultima_vez_display_actualizo = millis();
		Amostrando_humedad ? displayHum() : displayTemp();
	}
}

//==================================================================================================================//

void displayHum() // en "displayLecturas()"
{
	// limpiar display
	display.clearDisplay();

	// mostrar humedad aire
	display.setTextSize(1);
	display.setCursor(0, 0);
	display.print("Humedad del aire: ");
	display.setTextSize(2);
	display.setCursor(0, 10);
	display.print(humedad_aire_interior_promedio);
	display.print(" %");

	// mostrar humedad suelo
	display.setTextSize(1);
	display.setCursor(0, 35);
	display.print("Humedades del suelo: ");
	display.setTextSize(2);
	display.setCursor(0, 45);
	display.print(String(humedad_suelo_interior) + "  " + String(humedad_suelo_exterior));

	display.display();
}

//==================================================================================================================//

void displayTemp() // en "displayLecturas()"
{
	// limpiar display
	display.clearDisplay();

	// mostrar temperatura aire exterior
	display.setTextSize(1);
	display.setCursor(0, 0);
	display.print("Temp exterior: ");
	display.setTextSize(2);
	display.setCursor(0, 10);
	display.print(temp_exterior);
	display.print(" ");
	display.setTextSize(1);
	display.cp437(true);
	display.write(167);
	display.setTextSize(2);
	display.print("C");

	// mostrar temperatura aire interior
	display.setTextSize(1);
	display.setCursor(0, 35);
	display.print("Temp interior: ");
	display.setTextSize(2);
	display.setCursor(0, 45);
	display.print(temp_interior_promedio);
	display.print(" ");
	display.setTextSize(1);
	display.cp437(true);
	display.write(167);
	display.setTextSize(2);
	display.print("C");

	display.display();
}


#endif