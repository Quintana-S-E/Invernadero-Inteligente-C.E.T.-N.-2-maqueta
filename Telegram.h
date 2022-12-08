#ifndef Telegram_h
#define Telegram_h
#include "Claves.h"
#include "Declaraciones.h"
#include "Display.h"
#include "EEPROM_manejo.h"

void chequearMensajesRecibidosTelegram() // en "loop()"
{
	TBMessage mensaje;
	// mientras hayan nuevos mensajes
	while (CTBotMessageText == Bot.getNewMessage(mensaje))
	{
		String texto = mensaje.text;
		String nombre_solicitante = mensaje.sender.firstName;
		chat_rpta = ""; // borramos lo que hayamos escrito en llamadas anteriores
		// imprimir el mensaje recibido
		imprimirln("Mensaje obtenido:");
		imprimirln(texto);

		bool respondiendo_grupo;
		if (mensaje.sender.id != mensaje.group.id) // al no hablarle por grupo, mensaje.group.id = mensaje.sender.id
		{
			chat_id = mensaje.group.id;
			respondiendo_grupo = true;
		}
		else
		{
			chat_id = mensaje.sender.id;
			respondiendo_grupo = false;
		}

#ifdef CHAT_ID_AUTORIZADO
		// para comprobar la validez de la ID del solicitante
		if (mensaje.sender.id != CHAT_ID_AUTORIZADO)
		{
			Bot.sendMessage(chat_id, "Usuario no autorizado.");
			continue;
		}
#endif

		// si es el primer mensaje que recibe
		if (chat_primer_mensaje)
		{	// individual, o grupal siendo comando
			if (!respondiendo_grupo  ||  (respondiendo_grupo  &&  texto.charAt(0) == '/'))
			{
				chat_rpta = "Bienvenido, " + nombre_solicitante + ".\n";
				if (texto != "/start")
					chat_rpta += "Recuerde que puede enviar /start para ver los comandos";

				enviarMensaje(chat_id, chat_rpta);
				chat_primer_mensaje = false;
			}
		}
		chat_rpta = ""; // por seguridad
// XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX Acá evaluamos qué mensaje fue enviado XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX//
		if (texto == "/start")
			comandoStart();

		else if (texto == "/info")
			comandoInfo();
		
		else if (texto == "/lecturas")
			comandoLecturas();

		else if (texto == "/prog")
			comandoProg();

		else if (texto == "/ventilar")
			comandoVentilar();

		else if (texto == "/tiempoAl")
			comandoTiempoAl();

		else if (texto == "/tiempoRiego")
			comandoTiempoRiego();

		else if (texto == "/tiempoEspera")
			comandoTiempoEspera();

		else if (texto == "/tmax")
			comandoTmax();

		else if (texto == "/tmin")
			comandoTmin();

		else if (texto == "/tvent")
			comandoTvent();

		else if (texto == "/alarma")
			comandoAlarma();

		else if (texto == "/hum")
			comandoHum();

		else if (texto == "/led")
			comandoLed();

		else if (texto == "/reprog")
			comandoReprog();

		else if (texto == "/reprogramarEEPROM")
		{
			EEPROM_programada = false;
			escribirEEPROM(direccion[0], EEPROM_programada);
			chat_rpta = "Resetar para reprogramar la EEPROM";
		}

		else if (texto == "/uwu")
			chat_rpta = "Gracias 👉👈";

		else if (texto.charAt(0) == '/') // si solamente empieza por "/"
			chat_rpta = "El comando enviado no es válido. Envíe /start para ver las opciones";

		else // si el texto no es comando ni empieza con "/"
		{
			if (!respondiendo_grupo)
				chat_rpta = "No ha enviado un comando. Envíe\n/start para ver las opciones";
		}

		enviarMensaje(chat_id, chat_rpta);
	} // cierra el while() de "mientras haya nuevos mensajes, chequear su contenido"
} // cierra la función chequearMensajesRecibidosTelegram()


//=====================================================COMANDOS=====================================================//


void comandoStart()
{
	chat_rpta = "Puede enviar los siguientes comandos:\n\n";
	chat_rpta += "/info : Para revisar el estado del invernadero y los parámetros guardados\n";
	chat_rpta += "/lecturas : Para obtener datos actuales e históricos de los sensores\n";
	chat_rpta += "/prog : Para efectuar órdenes y modificaciones";
}

//==================================================================================================================//

void comandoInfo() // en "chequearMensajesRecibidosTelegram()"
{
	chat_rpta = "Estados:\n"; // ESTADOS
	chat_rpta += "-Alarma: ";
	alarma_activada ? (chat_rpta += "activada") : (chat_rpta += "desactivada");
	chat_rpta += ".\n";

	chat_rpta += "-Ventilación: ";
	ventilacion_forzada ? (chat_rpta += "forzada") : (chat_rpta += "en modo automático");
	chat_rpta += " y ";
	ventilando ? (chat_rpta += "activada") : (chat_rpta += "desactivada");
	chat_rpta += ".\n";

	chat_rpta += "-Tierra: ";
	if (esperando_riego)
		chat_rpta += "regada, esperando la absorción";
	else
		chat_rpta += "suficientemente húmeda";
	chat_rpta += ".\n";

	chat_rpta += "-LED: ";
	digitalRead(LED_ROJO) ? (chat_rpta += "encendido") : (chat_rpta += "apagado");
	chat_rpta += ".\n\n";

	chat_rpta += "\nParámetros:\n"; // PARÁMETROS
	chat_rpta += "-Humedad mínima del suelo: ";
	chat_rpta += String(humedad_suelo_minima);
	chat_rpta += " %.\n";

	chat_rpta += "-Lapso de envío de alarmas: ";
	int lapso_alarma_horas = lapso_alarma_minutos / 60;
	int lapso_alarma_horas_resto = lapso_alarma_minutos % 60;
	chat_rpta += String(lapso_alarma_horas);
	chat_rpta += " h";
	if (lapso_alarma_horas_resto != 0)
	{
		chat_rpta += " ";
		chat_rpta += String(lapso_alarma_horas_resto);
		chat_rpta += " min";
	}
	chat_rpta += ".\n";

	chat_rpta += "-Lapso de bombeo del riego: ";
	chat_rpta += String(tiempo_bombeo_segundos);
	chat_rpta += " segundos.\n";

	chat_rpta += "-Lapso de espera luego del riego: ";
	chat_rpta += String(tiempo_espera_minutos);
	chat_rpta += " min.\n";

	chat_rpta += "-Temperaturas de la alarma: ";
	chat_rpta += String(temp_minima_alarma);
	chat_rpta += " °C ";
	chat_rpta += "(mín), ";
	chat_rpta += String(temp_maxima_alarma);
	chat_rpta += " °C ";
	chat_rpta += "(máx).\n";

	chat_rpta += "-Temperatura de apertura de la ventilación: ";
	chat_rpta += String(temp_maxima_ventilacion);
	chat_rpta += " °C.";
}

//==================================================================================================================//

void comandoLecturas()
{// leerSensores() es llamada justo antes en el loop, así que no la llamamos para no retrasar
	chat_rpta = "Temperatura interior: " + String(temp_interior_promedio) + " °C\n";
	chat_rpta += "Humedad del suelo interior: " + String(humedad_suelo_interior) + " %\n";
	chat_rpta += "Humedad del aire interior: " + String(humedad_aire_interior_promedio) + " %\n";
	chat_rpta += "\n";
	chat_rpta += "Temperatura exterior: " + String(temp_exterior) + " °C\n";
	chat_rpta += "Humedad del suelo exterior: " + String(humedad_suelo_exterior) + " %\n";
	chat_rpta += "Humedad del aire exterior: " + String(humedad_aire_exterior) + " %\n";
	chat_rpta += "\nPara ver gráficos históricos de los datos ingrese al link:\n";
	chat_rpta += "https://thingspeak.com/channels/1937932";
}

//==================================================================================================================//

void comandoProg()
{
	chat_rpta = "Comandos de órdenes y modificaciones:\n\n";
	chat_rpta += "/alarma : Para activar/cancelar el envío periódico de la alarma de temperatura\n";
	chat_rpta += "/hum : Para cambiar el valor que define la necesidad de riego\n";
	chat_rpta += "/led : Para cambiar el estado del led\n";
	chat_rpta += "/tiempoAl : Para definir el lapso de tiempo de transmisión de la alarma de temperatura en modo automático\n";
	chat_rpta += "/tiempoRiego : Para definir el lapso de tiempo de bombeo del riego\n";
	chat_rpta += "/tiempoEspera : Para definir el tiempo de espera para volver a comprobar la necesidad de riego\n";
	chat_rpta += "/tmax : Para programar el valor de temperatura máxima para la alarma\n";
	chat_rpta += "/tmin : Para programar el valor de temperatura mínima para la alarma\n";
	chat_rpta += "/tvent : Para programar el valor de temperatura de activación de ventilación\n";
	chat_rpta += "/ventilar : Para forzar el cambio de la ventilación\n";
	chat_rpta += "/reprog : Para reiniciar los valores de los parámetros configurables";
	// chat_rpta += "/redpref :  Para elegir la red WiFi preferida\n";
	// chat_rpta += "/resettiempo :  Para volver a cero el contador de tiempo\n";
	// chat_rpta += "/resettemp :  Para resetear el registro de temperaturas max/min\n";
}

//==================================================================================================================//

void comandoVentilar()
{
	ventilacion_forzada = !ventilacion_forzada;
	ventilando = !ventilando;
	ventilando ? activarVentilacion() : desactivarVentilacion();

	chat_rpta = "La ventilación está ahora ";
	ventilando ? (chat_rpta += "activada") : (chat_rpta += "desactivada");
	chat_rpta += " (y en modo ";
	ventilacion_forzada ? (chat_rpta += "forzado)") : (chat_rpta += "automático)");
}

//==================================================================================================================//

void comandoTiempoAl()
{
	int lapso_alarma_horas = lapso_alarma_minutos / 60;
	int lapso_alarma_horas_resto = lapso_alarma_minutos % 60;
	chat_rpta = "El lapso de espera de la alarma está configurado en: ";
	chat_rpta += String(lapso_alarma_horas) + " h";
	if (lapso_alarma_horas_resto != 0)
	{
		chat_rpta += " ";
		chat_rpta += String(lapso_alarma_horas_resto);
		chat_rpta += " min";
	}
	enviarMensaje(chat_id, chat_rpta);

	chat_rpta = "Introduzca un nuevo valor en minutos (mayor a 0)"; // máx 65535 o 1092 horas o 45 días
	enviarMensaje(chat_id, chat_rpta);

	// setea chat_numero_entrada_int y la respuesta adecuada
	if (evaluarMensajeInt(1, 65535U, "minutos"))
	{
		lapso_alarma_minutos = chat_numero_entrada_int;
		escribirEEPROM(direccion[5], lapso_alarma_minutos);
	}
	// pedirlo en horas (1,5 horas, 3,5 horas, etc)
}

//==================================================================================================================//

void comandoTiempoRiego()
{
	chat_rpta = "El lapso de bombeo del riego está configurado en: ";
	chat_rpta += String(tiempo_bombeo_segundos) + " segundos";
	enviarMensaje(chat_id, chat_rpta);

	chat_rpta = "Introduzca un nuevo valor (entero, mayor a 0)"; // máx 65535 o 18,2 horas
	enviarMensaje(chat_id, chat_rpta);

	// setea chat_numero_entrada_int y la respuesta adecuada
	if (evaluarMensajeInt(1, 65535U, "segundos"))
	{
		tiempo_bombeo_segundos = chat_numero_entrada_int;
		escribirEEPROM(direccion[7], tiempo_bombeo_segundos);
	}
}

//==================================================================================================================//

void comandoTiempoEspera()
{
	chat_rpta = "El lapso de espera luego del riego está configurado en: ";
	chat_rpta += String(tiempo_espera_minutos) + " minutos";
	enviarMensaje(chat_id, chat_rpta);

	chat_rpta = "Introduzca un nuevo valor (entero, mayor a 0)"; // máx 65535 o 1092 horas o 45 días
	enviarMensaje(chat_id, chat_rpta);

	// setea chat_numero_entrada_int y la respuesta adecuada
	if (evaluarMensajeInt(1, 65535U, "minutos"))
	{
		tiempo_espera_minutos = chat_numero_entrada_int;
		escribirEEPROM(direccion[8], tiempo_espera_minutos);
	}
}

//==================================================================================================================//

void comandoTmax()
{
	chat_rpta = "La temperatura máxima de alarma está configurada en: ";
	chat_rpta += String(temp_maxima_alarma) + " °C";
	enviarMensaje(chat_id, chat_rpta);

	chat_rpta = "Introduzca un nuevo valor (mayor a ";
	chat_rpta += String(temp_minima_alarma) + ") ";
	chat_rpta += "entero, o si desea enviarlo con decimales, utilice punto (ej: 40.5)";
	enviarMensaje(chat_id, chat_rpta);

	// setea chat_numero_entrada_float y la respuesta adecuada
	if (evaluarMensajeFloat(temp_minima_alarma, 500.0F, "°C"))
	{
		temp_maxima_alarma = chat_numero_entrada_float;
		escribirEEPROM(direccion[1], temp_maxima_alarma);
	}
}

//==================================================================================================================//

void comandoTmin()
{
	chat_rpta = "La temperatura mínima de alarma está configurada en: ";
	chat_rpta += String(temp_minima_alarma) + " °C";
	enviarMensaje(chat_id, chat_rpta);

	chat_rpta = "Introduzca un nuevo valor (menor a ";
	chat_rpta += String(temp_maxima_alarma) + ") ";
	chat_rpta += "entero, o si desea enviarlo con decimales, utilice punto (ej: -4.5)";
	enviarMensaje(chat_id, chat_rpta);

	// setea chat_numero_entrada_float y la respuesta adecuada
	if (evaluarMensajeFloat(-500.0F, temp_maxima_alarma, "°C"))
	{
		temp_minima_alarma = chat_numero_entrada_float;
		escribirEEPROM(direccion[2], temp_minima_alarma);
	}
}

//==================================================================================================================//

void comandoTvent()
{
	chat_rpta = "La temperatura de activación de la ventilación está configurada en: ";
	chat_rpta += String(temp_maxima_ventilacion) + " °C";
	enviarMensaje(chat_id, chat_rpta);

	chat_rpta = "Introduzca un nuevo valor entero, o si desea enviarlo con decimales, utilice punto (ej: 35.4)";
	enviarMensaje(chat_id, chat_rpta);

	// setea chat_numero_entrada_float y la respuesta adecuada
	if (evaluarMensajeFloat(-500.0F, 500.0F, "°C"))
	{
		temp_maxima_ventilacion = chat_numero_entrada_float;
		escribirEEPROM(direccion[3], temp_maxima_ventilacion);
	}
}

//==================================================================================================================//

void comandoAlarma()
{
	alarma_activada = !alarma_activada;
	escribirEEPROM(direccion[6], alarma_activada);

	chat_rpta = "La alarma está ahora ";
	alarma_activada ? (chat_rpta += "activada") : (chat_rpta += "desactivada");
}

//==================================================================================================================//

void comandoHum()
{
	chat_rpta = "El valor de humedad mínimo está configurado en: ";
	chat_rpta += String(humedad_suelo_minima) + " %";
	enviarMensaje(chat_id, chat_rpta);

	chat_rpta = "Introduzca un nuevo valor (entero, de 0 a 100)";
	enviarMensaje(chat_id, chat_rpta);

	// setea chat_numero_entrada_int y la respuesta adecuada
	if (evaluarMensajeInt(0, 100, "%"))
	{
		humedad_suelo_minima = chat_numero_entrada_int;
		escribirEEPROM(direccion[4], humedad_suelo_minima);
	}
}

//==================================================================================================================//

void comandoLed()
{
	// leemos el estado actual del pin del LED y lo invertimos
	bool nuevo_estado_LED_rojo = !digitalRead(LED_ROJO);
	digitalWrite(LED_ROJO, nuevo_estado_LED_rojo); // para escribirlo al pin

	chat_rpta = "LED ";
	nuevo_estado_LED_rojo ? (chat_rpta += "encendido") : (chat_rpta += "apagado");
}

//==================================================================================================================//

void comandoReprog()
{
	cargarValoresPorDefecto();
	chat_rpta = "Se reiniciaron los valores de los parámetros guardados.\n";
	chat_rpta += "Puede verlos con /info";
}

//==================================================================================================================//

void enviarMensaje(uint64_t Aid, String Amensaje)
{
	Bot.sendMessage(Aid, Amensaje);
	imprimirln("Respuesta del BOT:");
	imprimirln(Amensaje);
}

//==================================================================================================================//

bool evaluarMensajeInt(uint16_t Avalor_min, uint16_t Avalor_max, String Aunidad)
{
	delay(5000); // esperamos 10 segundos para introducir una respuesta

	TBMessage msj;
	// si llegó un mensaje
	if (CTBotMessageText == Bot.getNewMessage(msj))
	{
		chat_numero_entrada_int = msj.text.toInt(); // pasamos el texto a número
		// si el número es válido
		if (chat_numero_entrada_int >= Avalor_min  &&  chat_numero_entrada_int <= Avalor_max)
		{
			chat_rpta = "Se ingresó: ";
			chat_rpta += String(chat_numero_entrada_int);
			chat_rpta += " ";
			chat_rpta += Aunidad;
			chat_rpta += ".\n\nEl valor se cambió exitosamente";
			return true; // devuelve siempre false, excepto que el número sea perfecto
		}

		chat_rpta = "El número ingresado no es válido. El valor no cambió.\n";
		chat_rpta += "Intente revisar los límites máximos y/o mínimos del valor (si los hubiera)";
	}
	else // si no llegó un mensaje
	{
		chat_rpta = "No se introdujo un número. El valor no cambió.\n";
		chat_rpta += "Cuando realiza un cambio, tiene 10 segundos para introducir su nuevo valor";
	}

	return false;
}

//==================================================================================================================//

bool evaluarMensajeFloat(float Avalor_min, float Avalor_max, String Aunidad)
{
	delay(5000); // esperamos 10 segundos para introducir una respuesta

	TBMessage msj;
	// si llegó un mensaje
	if (CTBotMessageText == Bot.getNewMessage(msj))
	{
		chat_numero_entrada_float = msj.text.toFloat(); // pasamos el texto a número
		// si el número es válido
		if (chat_numero_entrada_float >= Avalor_min  &&  chat_numero_entrada_float <= Avalor_max)
		{
			chat_rpta = "Se ingresó: ";
			chat_rpta += String(chat_numero_entrada_float);
			chat_rpta += " ";
			chat_rpta += Aunidad;
			chat_rpta += ".\n\nEl valor se cambió exitosamente";
			return true; // devuelve siempre false, excepto que el número sea perfecto
		}

		chat_rpta = "El número ingresado no es válido. El valor no cambió.\n";
		chat_rpta += "Intente revisar los límites máximos y/o mínimos del valor (si los hubiera)";
	}
	else // si no llegó un mensaje
	{
		chat_rpta = "No se introdujo un número. El valor no cambió.\n";
		chat_rpta += "Cuando realiza un cambio, tiene 10 segundos para introducir su nuevo valor";
	}

	return false;
}


//==================================================ALARMA Y WIFI===================================================//


void chequearAlarma() // en "loop()"
{
	// no hay problema con que lapso_alarma_minutos sea uint16_t, se multiplica por un UL
	if (millis() - ultima_vez_alarma_funciono >= (lapso_alarma_minutos * 60000UL)  &&  alarma_activada)
	{
		ultima_vez_alarma_funciono = millis();
		if (chat_id == 0)
			return; // el usuario debe hablar al Bot primeramente
		String mensaje;
		mensaje = "ALARMA: ";

		// evaluamos la temperatura
		if (temp_interior_promedio >= temp_maxima_alarma)
		{
			mensaje += "La temperatura del invernadero es excesivamente alta";
			enviarMensaje(chat_id, mensaje);
		}
		else if (temp_interior_promedio <= temp_minima_alarma)
		{
			mensaje += "La temperatura del invernadero es excesivamente baja";
			enviarMensaje(chat_id, mensaje);
		}
		
	}
}

//==================================================================================================================//

void chequearConexion()
{
	if (millis() - ultima_vez_comprobacion_WIFI >= DELAY_COMPROBACION_WIFI)
	{
		ultima_vez_comprobacion_WIFI = millis();

		// si no hay conexión con el bot, se fue el WiFi
		if (Bot.testConnection() != true)
		{
			// imprimir el error
			imprimirln("Bot desconectado. Reconectando...");
			// reconectar
			conectarWIFI(false);
		}
	}
}

//==================================================================================================================//

void conectarWIFI(bool parar_programa)
{
	displayConectandoWIFI();

	Bot.setMaxConnectionRetries(15);
	bool status_WIFI = false;
	// probar con cada red (imprime y muestra en el display si se conectó)
	if (!status_WIFI)
		status_WIFI = conectarWIFICon(SSIDescuela, PASSWORDescuela); // conectar al WIFI de la escuela
	if (!status_WIFI)
		status_WIFI = conectarWIFICon(SSIDnoni, PASSWORDnoni); // si no se pudo, probar con el de noni
	if (!status_WIFI)
		status_WIFI = conectarWIFICon(SSIDsanti, PASSWORDsanti); // si no se pudo, probar con el de santi
	if (!status_WIFI)
	{
		imprimirln("No se encuentra red WIFI.");
		displayErrorWIFI();
		if (parar_programa)
			while (1) ;
	}
	else
	{
		// conectar el bot a Telegram
		Bot.setTelegramToken(BOT_TOKEN);
		// inicializar ThingSpeak
		ThingSpeak.begin(Cliente);
	}
}

//==================================================================================================================//

bool conectarWIFICon(String Assid, String Apassword) // en "setup()"
{
	bool stat_WIFI;
	String res_WIFI;
	stat_WIFI = Bot.wifiConnect(Assid, Apassword);
	res_WIFI = stat_WIFI ? "Conectado a la red:\n" : "No se pudo conectar a: ";

	// imprimir el resultado de cada intento
	imprimirln(res_WIFI + String(Assid));
	// si se pudo conectar, imprimir un mensaje en el display (con el SSID conectado)
	if (stat_WIFI)
		displayConexionWIFI(res_WIFI, Assid);

	return stat_WIFI;
}

/*
// DEBUG ====================================================
//			imprimirln("Antes en la dirección 7 (alarma_activada) había un: ");
//			imprimirln(EEPROM.get(direccion[6], alarma_activada));
// DEBUG ====================================================
			comandoAlarma();
// DEBUG ====================================================
//			imprimirln("Ahora en la dirección 7 (alarma_activada) hay un: ");
//			imprimirln(EEPROM.get(direccion[6], alarma_activada));
// DEBUG ====================================================
*/

#endif