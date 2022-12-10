#pragma once

#include <Arduino.h>
#include <Adafruit_GFX.h>	  // display OLED
#include <Adafruit_Sensor.h>
#include <Adafruit_SSD1306.h> // display OLED
#include <ArduinoJson.h>
#include <CTBot.h>
#include <DHT.h>
#include <ESP32Servo.h>
#include <ThingSpeak.h>
#include <Wire.h> // I2C
#include <WiFi.h> // Gráficos.h

//#define DEBUGserial // Comentar para eliminar los Serial.print
#ifdef DEBUGserial
	#define imprimir(x) Serial.print(x)
	#define imprimirln(x) Serial.println(x)
#else
	#define imprimir(x)
	#define imprimirln(x)
#endif

// Otras constantes de funcionamiento
#define TELEGRAM_TIEMPO_MAX_CONFIGURACION 15000UL
#define DELAY_ACTIVIDAD_INVERNADERO 0UL // (ms) tiempo de espera para el loop del invernadero
#define DELAY_CAMBIO_DISPLAY 10000UL
#define DELAY_ACTUALIZACION_DISPLAY 500UL
#define DELAY_COMPROBACION_WIFI 60000UL // cada un minuto comprueba la conexión a WiFi
#define SCREEN_WIDTH 128			  // ancho del display OLED display, en píxeles
#define SCREEN_HEIGHT 64			  // alto del display OLED display, en píxeles
#define ANGULO_APERTURA 90			  // posición de apertura de la ventana
#define ANGULO_CERRADO 0			  // posición de cerrado de la ventana
#define MUESTRAS_HUMEDAD_SUELO 16	  // 16 máximo

// Pines de salida
#ifndef LED_BUILTIN
	#define LED_BUILTIN 2
#endif
#define LED_ROJO 4
#define PIN_BOMBA 18
#define PIN_VENTILADOR 19
#define PIN_SERVO 23

// Pines de los sensores DHT
#define DHT_INT_LOW_PIN 27
#define DHT_INT_HIGH_PIN 14
#define DHT_EXT_PIN 13
//#define DHT_INT_MID_PIN // Para el domo

// Pines de los sensores humedad suelo
#define SOIL_EXT_PIN A0
#define SOIL_INT_PIN A3
//#define SOIL_INT_2_PIN A6 // Para el domo

// ThingSpeak
#define FIELD_TEMP_INT 1
#define FIELD_TEMP_EXT 2
#define FIELD_HUM_AIRE_INT 3
#define FIELD_HUM_AIRE_EXT 4
#define FIELD_HUM_SUELO_INT 5
#define FIELD_HUM_SUELO_EXT 6

enum DatoMostradoEnDisplay
{
	Temperatura,
	Humedad
};
DatoMostradoEnDisplay DatoDelDisplay = Temperatura;

// Clases
CTBot Bot;
Servo Ventana;
WiFiClient Cliente;
Adafruit_SSD1306 Display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
DHT DhtInteriorLow(DHT_INT_LOW_PIN, DHT22);
DHT DhtInteriorHigh(DHT_INT_HIGH_PIN, DHT22);
DHT DhtExterior(DHT_EXT_PIN, DHT22);
//DHT DhtInteriorMid(DHT_INT_MID_PIN, DHT22); // Para el domo

// Variables de tiempo
unsigned long ultima_vez_invernadero_funciono = 0;
unsigned long ultima_vez_display_cambio = 0;
unsigned long ultima_vez_display_actualizo = 0;
unsigned long ultima_vez_thingspeak_actualizo = 0;
unsigned long ultima_vez_bomba_encendio = 0;
unsigned long ultima_vez_alarma_funciono = 0;
unsigned long ultima_vez_comprobacion_WIFI = 0;

// Flags de estado
bool ventilacion_forzada	= false; // si el estado de ventilación está siendo forzado por telegram
bool ventilando				= false;
bool esperando_riego		= false; // para chequearRiego()

// Datos de los sensores
// DHTs interiores
float temp_interior_promedio;
float humedad_aire_interior_promedio;
// DHTs exteriores
float temp_exterior;
float humedad_aire_exterior;
// soil moisture sensors
int humedad_suelo_interior;
int humedad_suelo_exterior;

// EEMPROM_manejo.h
void chequearEEPROMProgramada();
void setDireccionesEEPROM();
void leerEEPROMProgramada();
void cargarValoresPorDefecto();
void imprimirEEPROMValsDirsReads();
template <typename T>
void escribirEEPROM(int Adireccion, T Adato);
//template <typename T>
//T leerEEPROM(int Adireccion, T Adato);

// Sensores.h
void leerSensores();
void leerDHT22Interiores();
void leerSoilInteriores();
void leerDHT22Exteriores();
void leerSoilExteriores();

// Control.h
void chequearVentilacion();
void chequearRiego();
void activarVentilacion();
void desactivarVentilacion();
//void abrirVentana();
//void cerrarVentana();
//void chequear_iluminacion();

// Display.h
void inicializarDisplay();
void cambiarDatoDisplay();
void displayConectandoWIFI();
void displayErrorWIFI();
void displayConexionWIFI(String Amensaje_conectado_a, String Assid_conectada);
void actualizarDisplay();
void displayHum();
void displayTemp();

// Graficos.h
void actualizarGraficos();

// Telegram.h
// variables
String		chat_rpta; // necesariamente global para cambiarla en evaluarMensajeFloat() y evaluarMensajeInt()
uint64_t 	chat_id = 0; // comienza en 0 para comprobaciones en chequearAlarma()
uint16_t 	chat_numero_entrada_int;	// cuando preguntamos por un número entero de entrada
float		chat_numero_entrada_float;	// cuando preguntamos por un número con decimal de entrada
bool		chat_primer_mensaje = true; // para chequearMensajesRecibidosTelegram()
// WiFi
void conectarWIFI(bool parar_programa);
bool conectarWIFICon(String Assid, String Apassword);
void chequearConexion();
// funciones varias
void chequearMensajesRecibidosTelegram();
void chequearAlarma();
void enviarMensaje(uint64_t Aid, String Amensaje);
bool evaluarMensajeInt(uint16_t Avalor_min, uint16_t Avalor_max, String Aunidad);
bool evaluarMensajeFloat(float Avalor_min, float Avalor_max, String Aunidad);
// comandos
void comandoStart();
void comandoLecturas();
void comandoInfo();
void comandoProg();
void comandoVentilar();
void comandoTiempoAl();
void comandoTiempoRiego();
void comandoTiempoEspera();
void comandoTmax();
void comandoTmin();
void comandoTvent();
void comandoAlarma();
void comandoHum();
void comandoLed();
void comandoReprog();