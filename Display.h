#ifndef Display_h
#define Display_h
#include "Declaraciones.h"

void inicializarDisplay() // en "setup()"
{
	if (!Display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
	{
		imprimirln("Fallo en el display. Resetear el equipo.");
		while (1) ;
	}
	delay(2000);
	Display.clearDisplay();
	Display.setTextColor(WHITE);
}

//==================================================================================================================//

void displayConectandoWIFI() // en "setup()"
{
	Display.setTextSize(2);
	Display.setCursor(0, 0);
	Display.print("Conectando\na WIFI...");
	Display.display();
}

//==================================================================================================================//

void displayErrorWIFI() // en "setup()"
{
	Display.clearDisplay();
	Display.setTextSize(2);
	Display.setCursor(0, 0);
	Display.print("No se\nencuentra\nred WIFI.");
	Display.display();
}

//==================================================================================================================//

void displayConexionWIFI(String Amensaje_conectado_a, String Assid_conectada) // en "conectarWIFI()"
{
	Display.clearDisplay();
	Display.setTextSize(2);				// en grande:
	Display.setCursor(0, 0);
	Display.print(Amensaje_conectado_a);// conectado a la red
	Display.setTextSize(1);				// en chiquito:
	Display.setCursor(0, 40);
	Display.print(Assid_conectada);		// nombre de la red
	Display.display();
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
	Display.clearDisplay();

	// mostrar humedad aire
	Display.setTextSize(1);
	Display.setCursor(0, 0);
	Display.print("Humedad del aire: ");
	Display.setTextSize(2);
	Display.setCursor(0, 10);
	Display.print(humedad_aire_interior_promedio);
	Display.print(" %");

	// mostrar humedad suelo
	Display.setTextSize(1);
	Display.setCursor(0, 35);
	Display.print("Humedades del suelo: ");
	Display.setTextSize(2);
	Display.setCursor(0, 45);
	Display.print(String(humedad_suelo_interior) + " %  ");
	Display.print(String(humedad_suelo_exterior) + " %");

	Display.display();
}

//==================================================================================================================//

void displayTemp() // en "displayLecturas()"
{
	// limpiar display
	Display.clearDisplay();

	// mostrar temperatura aire exterior
	Display.setTextSize(1);
	Display.setCursor(0, 0);
	Display.print("Temp exterior: ");
	Display.setTextSize(2);
	Display.setCursor(0, 10);
	Display.print(temp_exterior);
	Display.print(" ");
	Display.setTextSize(1);
	Display.cp437(true);
	Display.write(167);
	Display.setTextSize(2);
	Display.print("C");

	// mostrar temperatura aire interior
	Display.setTextSize(1);
	Display.setCursor(0, 35);
	Display.print("Temp interior: ");
	Display.setTextSize(2);
	Display.setCursor(0, 45);
	Display.print(temp_interior_promedio);
	Display.print(" ");
	Display.setTextSize(1);
	Display.cp437(true);
	Display.write(167);
	Display.setTextSize(2);
	Display.print("C");

	Display.display();
}


#endif