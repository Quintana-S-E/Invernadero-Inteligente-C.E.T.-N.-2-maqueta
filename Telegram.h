#ifndef Telegram_h
#define Telegram_h
#include "Claves.h"
#include "Declaraciones.h"
#include "Display.h"
#include "EEPROM_manejo.h"

void chequearMensajesRecibidosTelegram() // en "loop()"
{
	TBMessage msj;
	// mientras hayan nuevos mensajes
	while (CTBotMessageText == Bot.getNewMessage(msj))
	{
		String texto = msj.text;
		String nombre_solicitante = msj.sender.firstName;
		telegram_respuesta = ""; // borramos lo que hayamos escrito en llamadas anteriores
		// imprimir el mensaje recibido
		imprimirln("Mensaje obtenido:");
		imprimirln(texto);

		bool respondiendo_grupo;
		if (msj.sender.id != msj.group.id) // si no se le habla por grupo, msj.group.id = msj.sender.id
		{
			telegram_chat_id = msj.group.id;
			respondiendo_grupo = true;
		}
		else
		{
			telegram_chat_id = msj.sender.id;
			respondiendo_grupo = false;
		}

		#ifdef CHAT_ID_AUTORIZADO
			// para comprobar la validez de la ID del solicitante
			if (msj.sender.id != CHAT_ID_AUTORIZADO)
			{
				Bot.sendMessage(telegram_chat_id, "Usuario no autorizado.");
				continue;
			}
		#endif

		// si es el primer mensaje que recibe
		if (telegram_primer_mensaje)
		{	// individual, o grupal siendo comando
			if (!respondiendo_grupo  ||  (respondiendo_grupo  &&  texto.charAt(0) == '/'))
			{
				telegram_respuesta = "Bienvenido, " + nombre_solicitante + ".\n";
				if (texto != "/start")
					telegram_respuesta += "Recuerde que puede enviar /start para ver los comandos";

				enviarMensaje(telegram_chat_id, telegram_respuesta);
				telegram_primer_mensaje = false;
			}
		}
		telegram_respuesta = ""; // por seguridad
// XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX Acá evaluamos qué mensaje fue enviado XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX//
		if (texto == "/start")
		{
			telegram_respuesta = "Puede enviar los siguientes comandos:\n\n";
			telegram_respuesta += "/info : Para revisar el estado del invernadero y los parámetros guardados\n";
			telegram_respuesta += "/lecturas : Para obtener datos actuales e históricos de los sensores\n";
			telegram_respuesta += "/prog : Para efectuar órdenes y modificaciones";
		}

		else if (texto == "/info")
		{
			telegram_respuesta = obtenerInfo();
		}

		else if (texto == "/lecturas")
		{// leerSensores() es llamada justo antes en el loop, así que no la llamamos para no retrasar
			telegram_respuesta = "Temperatura interior: " + String(temp_interior_promedio) + " °C\n";
			telegram_respuesta += "Humedad del suelo interior: " + String(humedad_suelo_interior) + " %\n";
			telegram_respuesta += "Humedad del aire interior: " + String(humedad_aire_interior_promedio) + " %\n";
			telegram_respuesta += "\n";
			telegram_respuesta += "Temperatura exterior: " + String(temp_exterior) + " °C\n";
			telegram_respuesta += "Humedad del suelo exterior: " + String(humedad_suelo_exterior) + " %\n";
			telegram_respuesta += "Humedad del aire exterior: " + String(humedad_aire_exterior) + " %\n";
			telegram_respuesta += "\nPara ver gráficos históricos de los datos ingrese al link:\n";
			telegram_respuesta += "https://thingspeak.com/channels/1937932";
		}

		else if (texto == "/prog")
		{
			telegram_respuesta = "Comandos de órdenes y modificaciones:\n\n";
			telegram_respuesta += "/alarma : Para activar/cancelar el envío periódico de la alarma de temperatura\n";
			telegram_respuesta += "/hum : Para cambiar el valor que define la necesidad de riego\n";
			telegram_respuesta += "/led : Para cambiar el estado del led\n";
			telegram_respuesta += "/tiempoAl : Para definir el lapso de tiempo de transmisión de la alarma de temperatura en modo automático\n";
			telegram_respuesta += "/tiempoRiego : Para definir el lapso de tiempo de bombeo del riego\n";
			telegram_respuesta += "/tiempoEspera : Para definir el tiempo de espera para volver a comprobar la necesidad de riego\n";
			telegram_respuesta += "/tmax : Para programar el valor de temperatura máxima para la alarma\n";
			telegram_respuesta += "/tmin : Para programar el valor de temperatura mínima para la alarma\n";
			telegram_respuesta += "/tvent : Para programar el valor de temperatura de activación de ventilación\n";
			telegram_respuesta += "/ventilar : Para forzar el cambio de la ventilación";
			// telegram_respuesta += "/redpref :  Para elegir la red WiFi preferida\n";
			// telegram_respuesta += "/resettiempo :  Para volver a cero el contador de tiempo\n";
			// telegram_respuesta += "/resettemp :  Para resetear el registro de temperaturas max/min\n";
		}

		else if (texto == "/reprogramarEEPROM")
		{
			EEPROM_programada = false;
			escribirEEPROM(direccion[0], EEPROM_programada);
			telegram_respuesta = "Resetar para reprogramar la EEPROM";
		}

		/*else if (texto == "/abrir") // para controlar ventana y ventiladores por separado
		{
			ventanaAbierta = !ventanaAbierta;
			ventanaAbierta ? (Ventana.write(ANGULO_APERTURA)) : (Ventana.write(ANGULO_CERRADO));

			telegram_respuesta = "La ventana está ahora ";
			ventanaAbierta ? (telegram_respuesta += "abierta.") : (telegram_respuesta += "cerrada.");
		}*/

		else if (texto == "/alarma")
		{
// DEBUG ====================================================
//			imprimirln("Antes en la dirección 7 (alarma_activada) había un: ");
//			imprimirln(EEPROM.get(direccion[6], alarma_activada));
// DEBUG ====================================================
			alarma_activada = !alarma_activada;
			escribirEEPROM(direccion[6], alarma_activada);

			telegram_respuesta = "La alarma está ahora ";
			alarma_activada ? (telegram_respuesta += "activada") : (telegram_respuesta += "desactivada");
// DEBUG ====================================================
//			imprimirln("Ahora en la dirección 7 (alarma_activada) hay un: ");
//			imprimirln(EEPROM.get(direccion[6], alarma_activada));
// DEBUG ====================================================
		}

		else if (texto == "/hum")
		{
			telegram_respuesta = "El valor de humedad mínimo está configurado en: ";
			telegram_respuesta += String(humedad_suelo_minima) + " %";
			enviarMensaje(telegram_chat_id, telegram_respuesta);

			telegram_respuesta = "Introduzca un nuevo valor (entero, de 0 a 100)";
			enviarMensaje(telegram_chat_id, telegram_respuesta);

			if (evaluarMensajeInt(0, 100, "%")) // setea telegram_respuesta_int y la telegram_respuesta adecuada
			{
				humedad_suelo_minima = telegram_respuesta_int;
				escribirEEPROM(direccion[4], humedad_suelo_minima);
			}
		}

		else if (texto == "/led")
		{
			// leemos el estado actual del pin del LED y lo invertimos
			bool nuevo_estado_LED_rojo = !digitalRead(LED_ROJO);
			digitalWrite(LED_ROJO, nuevo_estado_LED_rojo); // para escribirlo al pin

			telegram_respuesta = "LED ";
			nuevo_estado_LED_rojo ? (telegram_respuesta += "encendido") : (telegram_respuesta += "apagado");
		}

		else if (texto == "/tiempoAl")
		{
			int lapso_alarma_horas = lapso_alarma_minutos / 60;
			int lapso_alarma_horas_resto = lapso_alarma_minutos % 60;
			telegram_respuesta = "El lapso de espera de la alarma está configurado en: ";
			telegram_respuesta += String(lapso_alarma_horas) + " h";
			if (lapso_alarma_horas_resto != 0)
			{
				telegram_respuesta += " ";
				telegram_respuesta += String(lapso_alarma_horas_resto);
				telegram_respuesta += " min";
			}
			enviarMensaje(telegram_chat_id, telegram_respuesta);

			telegram_respuesta = "Introduzca un nuevo valor en minutos (mayor a 0)"; // máx 65535 o 1092 horas o 45 días
			enviarMensaje(telegram_chat_id, telegram_respuesta);

			if (evaluarMensajeInt(1, 65535U, "minutos")) // setea telegram_respuesta_int y la telegram_respuesta adecuada
			{
				lapso_alarma_minutos = telegram_respuesta_int;
				escribirEEPROM(direccion[5], lapso_alarma_minutos);
			}
			// pedirlo en horas (1,5 horas, 3,5 horas, etc)
		}

		else if (texto == "/tiempoRiego")
		{
			telegram_respuesta = "El lapso de bombeo del riego está configurado en: ";
			telegram_respuesta += String(tiempo_de_bomba_segundos) + " segundos";
			enviarMensaje(telegram_chat_id, telegram_respuesta);

			telegram_respuesta = "Introduzca un nuevo valor (entero, mayor a 0)"; // máx 65535 o 18,2 horas
			enviarMensaje(telegram_chat_id, telegram_respuesta);

			if (evaluarMensajeInt(1, 65535U, "segundos")) // setea telegram_respuesta_int y la telegram_respuesta adecuada
			{
				tiempo_de_bomba_segundos = telegram_respuesta_int;
				escribirEEPROM(direccion[7], tiempo_de_bomba_segundos);
			}
		}

		else if (texto == "/tiempoEspera")
		{
			telegram_respuesta = "El lapso de espera luego del riego está configurado en: ";
			telegram_respuesta += String(tiempo_espera_minutos) + " minutos";
			enviarMensaje(telegram_chat_id, telegram_respuesta);

			telegram_respuesta = "Introduzca un nuevo valor (entero, mayor a 0)"; // máx 65535 o 1092 horas o 45 días
			enviarMensaje(telegram_chat_id, telegram_respuesta);

			if (evaluarMensajeInt(1, 65535U, "minutos")) // setea telegram_respuesta_int y la telegram_respuesta adecuada
			{
				tiempo_espera_minutos = telegram_respuesta_int;
				escribirEEPROM(direccion[8], tiempo_espera_minutos);
			}
		}

		else if (texto == "/tmax")
		{
			telegram_respuesta = "La temperatura máxima de alarma está configurada en: ";
			telegram_respuesta += String(temp_maxima_alarma) + " °C";
			enviarMensaje(telegram_chat_id, telegram_respuesta);

			telegram_respuesta = "Introduzca un nuevo valor (mayor a ";
			telegram_respuesta += String(temp_minima_alarma) + ") ";
			telegram_respuesta += "entero, o si desea enviarlo con decimales, utilice punto (ej: 40.5).";
			enviarMensaje(telegram_chat_id, telegram_respuesta);

			if (evaluarMensajeFloat(temp_minima_alarma, 500.0F, "°C")) // setea telegram_respuesta_int y la telegram_respuesta adecuada
			{
				temp_maxima_alarma = telegram_respuesta_float;
				escribirEEPROM(direccion[1], temp_maxima_alarma);
			}
		}

		else if (texto == "/tmin")
		{
			telegram_respuesta = "La temperatura mínima de alarma está configurada en: ";
			telegram_respuesta += String(temp_minima_alarma) + " °C";
			enviarMensaje(telegram_chat_id, telegram_respuesta);

			telegram_respuesta = "Introduzca un nuevo valor (menor a ";
			telegram_respuesta += String(temp_maxima_alarma) + ") ";
			telegram_respuesta += "entero, o si desea enviarlo con decimales, utilice punto (ej: -4.5)";
			enviarMensaje(telegram_chat_id, telegram_respuesta);

			if (evaluarMensajeFloat(-500.0F, temp_maxima_alarma, "°C")) // setea telegram_respuesta_int y la telegram_respuesta adecuada
			{
				temp_minima_alarma = telegram_respuesta_float;
				escribirEEPROM(direccion[2], temp_minima_alarma);
			}
		}

		else if (texto == "/tvent")
		{
			telegram_respuesta = "La temperatura de activación de la ventilación está configurada en: ";
			telegram_respuesta += String(temp_maxima_ventilacion) + " °C";
			enviarMensaje(telegram_chat_id, telegram_respuesta);

			telegram_respuesta = "Introduzca un nuevo valor entero, o si desea enviarlo con decimales, utilice punto (ej: 35.4)";
			enviarMensaje(telegram_chat_id, telegram_respuesta);

			if (evaluarMensajeFloat(-500.0F, 500.0F, "°C")) // setea telegram_respuesta_int y la telegram_respuesta adecuada
			{
				temp_maxima_ventilacion = telegram_respuesta_float;
				escribirEEPROM(direccion[3], temp_maxima_ventilacion);
			}
		}

		else if (texto == "/ventilar")
		{
			ventilacion_forzada = !ventilacion_forzada;
			ventilando = !ventilando;
			ventilando ? activarVentilacion() : desactivarVentilacion();

			telegram_respuesta = "La ventilación está ahora ";
			ventilando ? (telegram_respuesta += "activada") : (telegram_respuesta += "desactivada");
			telegram_respuesta += " (y en modo ";
			ventilacion_forzada ? (telegram_respuesta += "forzado)") : (telegram_respuesta += "automático)");
		}

		else if (texto == "/uwu")
		{
			telegram_respuesta = "Gracias 👉👈";
		}

		else if (texto.charAt(0) == '/') // si solamente empieza por "/"
		{
			telegram_respuesta = "El comando enviado no es válido. Envíe /start para ver las opciones";
		}

		else // si el texto no es comando ni empieza con "/"
		{
			if (!respondiendo_grupo)
				telegram_respuesta = "No ha enviado un comando. Envíe\n/start para ver las opciones";
		}

		enviarMensaje(telegram_chat_id, telegram_respuesta);
	} // cierra el while() de "mientras haya nuevos mensajes, chequear su contenido"
} // cierra la función chequearMensajesRecibidosTelegram()

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
	delay(10000); // esperamos 10 segundos para introducir una respuesta

	TBMessage msj_FB;
	// si llegó un mensaje
	if (CTBotMessageText == Bot.getNewMessage(msj_FB))
	{
		telegram_respuesta_int = msj_FB.text.toInt(); // pasamos el texto a número
		// si el número es válido
		if (telegram_respuesta_int >= Avalor_min  &&  telegram_respuesta_int <= Avalor_max)
		{
			telegram_respuesta = "Se ingresó: ";
			telegram_respuesta += String(telegram_respuesta_int);
			telegram_respuesta += " ";
			telegram_respuesta += Aunidad;
			telegram_respuesta += ".\n\nEl valor se cambió exitosamente";
			return true; // devuelve siempre false, excepto que el número sea perfecto
		}
		else
		{
			telegram_respuesta = "El número ingresado no es válido. El valor no cambió.\n";
			telegram_respuesta += "Intente revisar los límites máximos y/o mínimos del valor (si los hubiera).";
		}
	}
	else // si no llegó un mensaje
	{
		telegram_respuesta = "No se introdujo un número. El valor no cambió.\n";
		telegram_respuesta += "Cuando realiza un cambio, tiene 10 segundos para introducir su nuevo valor";
	}

	return false;
}

//==================================================================================================================//

bool evaluarMensajeFloat(float Avalor_min, float Avalor_max, String Aunidad)
{
	delay(10000); // esperamos 10 segundos para introducir una respuesta

	TBMessage msj_FB;
	// si llegó un mensaje
	if (CTBotMessageText == Bot.getNewMessage(msj_FB))
	{
		telegram_respuesta_float = msj_FB.text.toFloat(); // pasamos el texto a número
		// si el número es válido
		if (telegram_respuesta_float >= Avalor_min  &&  telegram_respuesta_float <= Avalor_max)
		{
			telegram_respuesta = "Se ingresó: ";
			telegram_respuesta += String(telegram_respuesta_float);
			telegram_respuesta += " ";
			telegram_respuesta += Aunidad;
			telegram_respuesta += ".\n\nEl valor se cambió exitosamente";
			return true; // devuelve siempre false, excepto que el número sea perfecto
		}
		else
		{
			telegram_respuesta = "El número ingresado no es válido. El valor no cambió.\n";
			telegram_respuesta += "Intente revisar los límites máximos y/o mínimos del valor (si los hubiera).";
		}
	}
	else // si no llegó un mensaje
	{
		telegram_respuesta = "No se introdujo un número. El valor no cambió.\n";
		telegram_respuesta += "Cuando realiza un cambio, tiene 10 segundos para introducir su nuevo valor";
	}

	return false;
}

//==================================================================================================================//

String obtenerInfo() // en "chequearMensajesRecibidosTelegram()"
{
	String msj_FS;
	msj_FS = "Estados:\n"; // ESTADOS
	msj_FS += "-Alarma: ";
	alarma_activada ? (msj_FS += "activada") : (msj_FS += "desactivada");
	msj_FS += ".\n";

	msj_FS += "-Ventilación: ";
	ventilacion_forzada ? (msj_FS += "forzada") : (msj_FS += "en modo automático");
	msj_FS += " y ";
	ventilando ? (msj_FS += "activada") : (msj_FS += "desactivada");
	msj_FS += ".\n";

	msj_FS += "-Tierra: ";
	if (esperando_riego)
		msj_FS += "regada, esperando la absorción";
	else
		msj_FS += "suficientemente húmeda";
	msj_FS += ".\n";

	msj_FS += "-LED: ";
	digitalRead(LED_ROJO) ? (msj_FS += "encendido") : (msj_FS += "apagado");
	msj_FS += ".\n\n";

	msj_FS += "\nParámetros:\n"; // PARÁMETROS
	msj_FS += "-Humedad mínima del suelo: ";
	msj_FS += String(humedad_suelo_minima);
	msj_FS += " %.\n";

	msj_FS += "-Lapso de envío de alarmas: ";
	int lapso_alarma_horas = lapso_alarma_minutos / 60;
	int lapso_alarma_horas_resto = lapso_alarma_minutos % 60;
	msj_FS += String(lapso_alarma_horas);
	msj_FS += " h";
	if (lapso_alarma_horas_resto != 0)
	{
		msj_FS += " ";
		msj_FS += String(lapso_alarma_horas_resto);
		msj_FS += " min";
	}
	msj_FS += ".\n";

	msj_FS += "-Lapso de bombeo del riego: ";
	msj_FS += String(tiempo_de_bomba_segundos);
	msj_FS += " segundos.\n";

	msj_FS += "-Lapso de espera luego del riego: ";
	msj_FS += String(tiempo_espera_minutos);
	msj_FS += " min.\n";

	msj_FS += "-Temperaturas de la alarma: ";
	msj_FS += String(temp_minima_alarma);
	msj_FS += " °C ";
	msj_FS += "(mín), ";
	msj_FS += String(temp_maxima_alarma);
	msj_FS += " °C ";
	msj_FS += "(máx).\n";

	msj_FS += "-Temperatura de apertura de la ventilación: ";
	msj_FS += String(temp_maxima_ventilacion);
	msj_FS += " °C.";

	return msj_FS;
}

//==================================================================================================================//

void chequearAlarma() // en "loop()"
{
	// no hay problema con que lapso_alarma_minutos sea uint16_t, se multiplica por un UL
	if (millis() - ultima_vez_alarma_funciono >= (lapso_alarma_minutos * 60000UL)  &&  alarma_activada)
	{
		ultima_vez_alarma_funciono = millis();
		if (telegram_chat_id == 0)
			return; // el usuario debe hablar al Bot primeramente
		String mensaje;
		mensaje = "ALARMA: ";

		// evaluamos la temperatura
		if (temp_interior_promedio >= temp_maxima_alarma)
		{
			mensaje += "La temperatura del invernadero es excesivamente alta";
			enviarMensaje(telegram_chat_id, mensaje);
		}
		else if (temp_interior_promedio <= temp_minima_alarma)
		{
			mensaje += "La temperatura del invernadero es excesivamente baja";
			enviarMensaje(telegram_chat_id, mensaje);
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


#endif