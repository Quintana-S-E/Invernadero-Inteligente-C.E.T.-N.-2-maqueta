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
		CT.respuesta = ""; // borramos lo que hayamos escrito en llamadas anteriores
		// imprimir el mensaje recibido
		imprimirln("Mensaje obtenido:");
		imprimirln(texto);

		bool respondiendo_grupo;
		if (msj.sender.id != msj.group.id) // si no se le habla por grupo, msj.group.id = msj.sender.id
		{
			CT.id = msj.group.id;
			respondiendo_grupo = true;
		}
		else
		{
			CT.id = msj.sender.id;
			respondiendo_grupo = false;
		}

#ifdef CHAT_ID_AUTORIZADO
		// para comprobar la validez de la ID del solicitante
		if (msj.sender.id != CHAT_ID_AUTORIZADO)
		{
			Bot.sendMessage(CT.id, "Usuario no autorizado.");
			continue;
		}
#endif

		// si es el primer mensaje que recibe
		if (telegram_primer_mensaje)
		{	// individual, o grupal siendo comando
			if (!respondiendo_grupo  ||  (respondiendo_grupo  &&  texto.charAt(0) == '/'))
			{
				CT.respuesta = "Bienvenido, " + nombre_solicitante + ".\n";
				if (texto != "/start")
					CT.respuesta += "Recuerde que puede enviar /start para ver los comandos";

				CT.enviarMensaje(CT.id, CT.respuesta);
				telegram_primer_mensaje = false;
			}
		}
		CT.respuesta = ""; // por seguridad
// XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX Acá evaluamos qué mensaje fue enviado XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX//
		if (texto == "/start")
			CT.comandoStart();

		else if (texto == "/info")
			CT.comandoInfo();
		
		else if (texto == "/lecturas")
			CT.comandoLecturas();

		else if (texto == "/prog")
			CT.comandoProg();

		else if (texto == "/ventilar")
			CT.comandoVentilar();

		else if (texto == "/tiempoAl")
			CT.comandoTiempoAl();

		else if (texto == "/tiempoRiego")
			CT.comandoTiempoRiego();

		else if (texto == "/tiempoEspera")
			CT.comandoTiempoEspera();

		else if (texto == "/tmax")
			CT.comandoTmax();

		else if (texto == "/tmin")
			CT.comandoTmin();

		else if (texto == "/tvent")
			CT.comandoTvent();

		else if (texto == "/alarma")
			CT.comandoAlarma();

		else if (texto == "/hum")
			CT.comandoHum();

		else if (texto == "/led")
			CT.comandoLed();

		else if (texto == "/reprog")
			CT.comandoReprog();

		else if (texto == "/reprogramarEEPROM")
		{
			EEPROM_programada = false;
			escribirEEPROM(direccion[0], EEPROM_programada);
			CT.respuesta = "Resetar para reprogramar la EEPROM";
		}

		else if (texto == "/uwu")
			CT.respuesta = "Gracias 👉👈";

		else if (texto.charAt(0) == '/') // si solamente empieza por "/"
			CT.respuesta = "El comando enviado no es válido. Envíe /start para ver las opciones";

		else // si el texto no es comando ni empieza con "/"
		{
			if (!respondiendo_grupo)
				CT.respuesta = "No ha enviado un comando. Envíe\n/start para ver las opciones";
		}

		CT.enviarMensaje(CT.id, CT.respuesta);
	} // cierra el while() de "mientras haya nuevos mensajes, chequear su contenido"
} // cierra la función chequearMensajesRecibidosTelegram()


//=====================================================COMANDOS=====================================================//


void InteraccionTelegram::comandoStart()
{
	CT.respuesta = "Puede enviar los siguientes comandos:\n\n";
	CT.respuesta += "/info : Para revisar el estado del invernadero y los parámetros guardados\n";
	CT.respuesta += "/lecturas : Para obtener datos actuales e históricos de los sensores\n";
	CT.respuesta += "/prog : Para efectuar órdenes y modificaciones";
}

//==================================================================================================================//

void InteraccionTelegram::comandoInfo() // en "chequearMensajesRecibidosTelegram()"
{
	CT.respuesta = "Estados:\n"; // ESTADOS
	CT.respuesta += "-Alarma: ";
	alarma_activada ? (CT.respuesta += "activada") : (CT.respuesta += "desactivada");
	CT.respuesta += ".\n";

	CT.respuesta += "-Ventilación: ";
	ventilacion_forzada ? (CT.respuesta += "forzada") : (CT.respuesta += "en modo automático");
	CT.respuesta += " y ";
	ventilando ? (CT.respuesta += "activada") : (CT.respuesta += "desactivada");
	CT.respuesta += ".\n";

	CT.respuesta += "-Tierra: ";
	if (esperando_riego)
		CT.respuesta += "regada, esperando la absorción";
	else
		CT.respuesta += "suficientemente húmeda";
	CT.respuesta += ".\n";

	CT.respuesta += "-LED: ";
	digitalRead(LED_ROJO) ? (CT.respuesta += "encendido") : (CT.respuesta += "apagado");
	CT.respuesta += ".\n\n";

	CT.respuesta += "\nParámetros:\n"; // PARÁMETROS
	CT.respuesta += "-Humedad mínima del suelo: ";
	CT.respuesta += String(humedad_suelo_minima);
	CT.respuesta += " %.\n";

	CT.respuesta += "-Lapso de envío de alarmas: ";
	int lapso_alarma_horas = lapso_alarma_minutos / 60;
	int lapso_alarma_horas_resto = lapso_alarma_minutos % 60;
	CT.respuesta += String(lapso_alarma_horas);
	CT.respuesta += " h";
	if (lapso_alarma_horas_resto != 0)
	{
		CT.respuesta += " ";
		CT.respuesta += String(lapso_alarma_horas_resto);
		CT.respuesta += " min";
	}
	CT.respuesta += ".\n";

	CT.respuesta += "-Lapso de bombeo del riego: ";
	CT.respuesta += String(tiempo_bombeo_segundos);
	CT.respuesta += " segundos.\n";

	CT.respuesta += "-Lapso de espera luego del riego: ";
	CT.respuesta += String(tiempo_espera_minutos);
	CT.respuesta += " min.\n";

	CT.respuesta += "-Temperaturas de la alarma: ";
	CT.respuesta += String(temp_minima_alarma);
	CT.respuesta += " °C ";
	CT.respuesta += "(mín), ";
	CT.respuesta += String(temp_maxima_alarma);
	CT.respuesta += " °C ";
	CT.respuesta += "(máx).\n";

	CT.respuesta += "-Temperatura de apertura de la ventilación: ";
	CT.respuesta += String(temp_maxima_ventilacion);
	CT.respuesta += " °C.";
}

//==================================================================================================================//

void InteraccionTelegram::comandoLecturas()
{// leerSensores() es llamada justo antes en el loop, así que no la llamamos para no retrasar
	CT.respuesta = "Temperatura interior: " + String(temp_interior_promedio) + " °C\n";
	CT.respuesta += "Humedad del suelo interior: " + String(humedad_suelo_interior) + " %\n";
	CT.respuesta += "Humedad del aire interior: " + String(humedad_aire_interior_promedio) + " %\n";
	CT.respuesta += "\n";
	CT.respuesta += "Temperatura exterior: " + String(temp_exterior) + " °C\n";
	CT.respuesta += "Humedad del suelo exterior: " + String(humedad_suelo_exterior) + " %\n";
	CT.respuesta += "Humedad del aire exterior: " + String(humedad_aire_exterior) + " %\n";
	CT.respuesta += "\nPara ver gráficos históricos de los datos ingrese al link:\n";
	CT.respuesta += "https://thingspeak.com/channels/1937932";
}

//==================================================================================================================//

void InteraccionTelegram::comandoProg()
{
	CT.respuesta = "Comandos de órdenes y modificaciones:\n\n";
	CT.respuesta += "/alarma : Para activar/cancelar el envío periódico de la alarma de temperatura\n";
	CT.respuesta += "/hum : Para cambiar el valor que define la necesidad de riego\n";
	CT.respuesta += "/led : Para cambiar el estado del led\n";
	CT.respuesta += "/tiempoAl : Para definir el lapso de tiempo de transmisión de la alarma de temperatura en modo automático\n";
	CT.respuesta += "/tiempoRiego : Para definir el lapso de tiempo de bombeo del riego\n";
	CT.respuesta += "/tiempoEspera : Para definir el tiempo de espera para volver a comprobar la necesidad de riego\n";
	CT.respuesta += "/tmax : Para programar el valor de temperatura máxima para la alarma\n";
	CT.respuesta += "/tmin : Para programar el valor de temperatura mínima para la alarma\n";
	CT.respuesta += "/tvent : Para programar el valor de temperatura de activación de ventilación\n";
	CT.respuesta += "/ventilar : Para forzar el cambio de la ventilación\n";
	CT.respuesta += "/reprog : Para reiniciar los valores de los parámetros configurables";
	// CT.respuesta += "/redpref :  Para elegir la red WiFi preferida\n";
	// CT.respuesta += "/resettiempo :  Para volver a cero el contador de tiempo\n";
	// CT.respuesta += "/resettemp :  Para resetear el registro de temperaturas max/min\n";
}

//==================================================================================================================//

void InteraccionTelegram::comandoVentilar()
{
	ventilacion_forzada = !ventilacion_forzada;
	ventilando = !ventilando;
	ventilando ? activarVentilacion() : desactivarVentilacion();

	CT.respuesta = "La ventilación está ahora ";
	ventilando ? (CT.respuesta += "activada") : (CT.respuesta += "desactivada");
	CT.respuesta += " (y en modo ";
	ventilacion_forzada ? (CT.respuesta += "forzado)") : (CT.respuesta += "automático)");
}

//==================================================================================================================//

void InteraccionTelegram::comandoTiempoAl()
{
	int lapso_alarma_horas = lapso_alarma_minutos / 60;
	int lapso_alarma_horas_resto = lapso_alarma_minutos % 60;
	CT.respuesta = "El lapso de espera de la alarma está configurado en: ";
	CT.respuesta += String(lapso_alarma_horas) + " h";
	if (lapso_alarma_horas_resto != 0)
	{
		CT.respuesta += " ";
		CT.respuesta += String(lapso_alarma_horas_resto);
		CT.respuesta += " min";
	}
	CT.enviarMensaje(CT.id, CT.respuesta);

	CT.respuesta = "Introduzca un nuevo valor en minutos (mayor a 0)"; // máx 65535 o 1092 horas o 45 días
	CT.enviarMensaje(CT.id, CT.respuesta);

	// setea numero_entrada_int y respuesta adecuada
	if (CT.evaluarMensajeInt(1, 65535U, "minutos"))
	{
		lapso_alarma_minutos = CT.numero_entrada_int;
		escribirEEPROM(direccion[5], lapso_alarma_minutos);
	}
	// pedirlo en horas (1,5 horas, 3,5 horas, etc)
}

//==================================================================================================================//

void InteraccionTelegram::comandoTiempoRiego()
{
	CT.respuesta = "El lapso de bombeo del riego está configurado en: ";
	CT.respuesta += String(tiempo_bombeo_segundos) + " segundos";
	CT.enviarMensaje(CT.id, CT.respuesta);

	CT.respuesta = "Introduzca un nuevo valor (entero, mayor a 0)"; // máx 65535 o 18,2 horas
	CT.enviarMensaje(CT.id, CT.respuesta);

	// setea numero_entrada_int y respuesta adecuada
	if (CT.evaluarMensajeInt(1, 65535U, "segundos"))
	{
		tiempo_bombeo_segundos = CT.numero_entrada_int;
		escribirEEPROM(direccion[7], tiempo_bombeo_segundos);
	}
}

//==================================================================================================================//

void InteraccionTelegram::comandoTiempoEspera()
{
	CT.respuesta = "El lapso de espera luego del riego está configurado en: ";
	CT.respuesta += String(tiempo_espera_minutos) + " minutos";
	CT.enviarMensaje(CT.id, CT.respuesta);

	CT.respuesta = "Introduzca un nuevo valor (entero, mayor a 0)"; // máx 65535 o 1092 horas o 45 días
	CT.enviarMensaje(CT.id, CT.respuesta);

	// setea numero_entrada_int y respuesta adecuada
	if (CT.evaluarMensajeInt(1, 65535U, "minutos"))
	{
		tiempo_espera_minutos = CT.numero_entrada_int;
		escribirEEPROM(direccion[8], tiempo_espera_minutos);
	}
}

//==================================================================================================================//

void InteraccionTelegram::comandoTmax()
{
	CT.respuesta = "La temperatura máxima de alarma está configurada en: ";
	CT.respuesta += String(temp_maxima_alarma) + " °C";
	CT.enviarMensaje(CT.id, CT.respuesta);

	CT.respuesta = "Introduzca un nuevo valor (mayor a ";
	CT.respuesta += String(temp_minima_alarma) + ") ";
	CT.respuesta += "entero, o si desea enviarlo con decimales, utilice punto (ej: 40.5)";
	CT.enviarMensaje(CT.id, CT.respuesta);

	// setea numero_entrada_float y respuesta adecuada
	if (CT.evaluarMensajeFloat(temp_minima_alarma, 500.0F, "°C"))
	{
		temp_maxima_alarma = CT.numero_entrada_float;
		escribirEEPROM(direccion[1], temp_maxima_alarma);
	}
}

//==================================================================================================================//

void InteraccionTelegram::comandoTmin()
{
	CT.respuesta = "La temperatura mínima de alarma está configurada en: ";
	CT.respuesta += String(temp_minima_alarma) + " °C";
	CT.enviarMensaje(CT.id, CT.respuesta);

	CT.respuesta = "Introduzca un nuevo valor (menor a ";
	CT.respuesta += String(temp_maxima_alarma) + ") ";
	CT.respuesta += "entero, o si desea enviarlo con decimales, utilice punto (ej: -4.5)";
	CT.enviarMensaje(CT.id, CT.respuesta);

	// setea numero_entrada_float y la respuesta adecuada
	if (CT.evaluarMensajeFloat(-500.0F, temp_maxima_alarma, "°C"))
	{
		temp_minima_alarma = CT.numero_entrada_float;
		escribirEEPROM(direccion[2], temp_minima_alarma);
	}
}

//==================================================================================================================//

void InteraccionTelegram::comandoTvent()
{
	CT.respuesta = "La temperatura de activación de la ventilación está configurada en: ";
	CT.respuesta += String(temp_maxima_ventilacion) + " °C";
	CT.enviarMensaje(CT.id, CT.respuesta);

	CT.respuesta = "Introduzca un nuevo valor entero, o si desea enviarlo con decimales, utilice punto (ej: 35.4)";
	CT.enviarMensaje(CT.id, CT.respuesta);

	// setea numero_entrada_float y la respuesta adecuada
	if (CT.evaluarMensajeFloat(-500.0F, 500.0F, "°C"))
	{
		temp_maxima_ventilacion = CT.numero_entrada_float;
		escribirEEPROM(direccion[3], temp_maxima_ventilacion);
	}
}

//==================================================================================================================//

void InteraccionTelegram::comandoAlarma()
{
	alarma_activada = !alarma_activada;
	escribirEEPROM(direccion[6], alarma_activada);

	CT.respuesta = "La alarma está ahora ";
	alarma_activada ? (CT.respuesta += "activada") : (CT.respuesta += "desactivada");
}

//==================================================================================================================//

void InteraccionTelegram::comandoHum()
{
	CT.respuesta = "El valor de humedad mínimo está configurado en: ";
	CT.respuesta += String(humedad_suelo_minima) + " %";
	CT.enviarMensaje(CT.id, CT.respuesta);

	CT.respuesta = "Introduzca un nuevo valor (entero, de 0 a 100)";
	CT.enviarMensaje(CT.id, CT.respuesta);

	// setea numero_entrada_int y respuesta adecuada
	if (CT.evaluarMensajeInt(0, 100, "%"))
	{
		humedad_suelo_minima = CT.numero_entrada_int;
		escribirEEPROM(direccion[4], humedad_suelo_minima);
	}
}

//==================================================================================================================//

void InteraccionTelegram::comandoLed()
{
	// leemos el estado actual del pin del LED y lo invertimos
	bool nuevo_estado_LED_rojo = !digitalRead(LED_ROJO);
	digitalWrite(LED_ROJO, nuevo_estado_LED_rojo); // para escribirlo al pin

	CT.respuesta = "LED ";
	nuevo_estado_LED_rojo ? (CT.respuesta += "encendido") : (CT.respuesta += "apagado");
}

//==================================================================================================================//

void InteraccionTelegram::comandoReprog()
{
	cargarValoresPorDefecto();
	CT.respuesta = "Se reiniciaron los valores de los parámetros guardados.\n";
	CT.respuesta += "Puede verlos con /info";
}

//==================================================================================================================//

void InteraccionTelegram::enviarMensaje(uint64_t Aid, String Amensaje)
{
	Bot.sendMessage(Aid, Amensaje);
	imprimirln("Respuesta del BOT:");
	imprimirln(Amensaje);
}

//==================================================================================================================//

bool InteraccionTelegram::evaluarMensajeInt(uint16_t Avalor_min, uint16_t Avalor_max, String Aunidad)
{
	delay(10000); // esperamos 10 segundos para introducir una respuesta

	TBMessage msj_FB;
	// si llegó un mensaje
	if (CTBotMessageText == Bot.getNewMessage(msj_FB))
	{
		CT.numero_entrada_int = msj_FB.text.toInt(); // pasamos el texto a número
		// si el número es válido
		if (CT.numero_entrada_int >= Avalor_min  &&  CT.numero_entrada_int <= Avalor_max)
		{
			CT.respuesta = "Se ingresó: ";
			CT.respuesta += String(CT.numero_entrada_int);
			CT.respuesta += " ";
			CT.respuesta += Aunidad;
			CT.respuesta += ".\n\nEl valor se cambió exitosamente";
			return true; // devuelve siempre false, excepto que el número sea perfecto
		}
		else
		{
			CT.respuesta = "El número ingresado no es válido. El valor no cambió.\n";
			CT.respuesta += "Intente revisar los límites máximos y/o mínimos del valor (si los hubiera)";
		}
	}
	else // si no llegó un mensaje
	{
		CT.respuesta = "No se introdujo un número. El valor no cambió.\n";
		CT.respuesta += "Cuando realiza un cambio, tiene 10 segundos para introducir su nuevo valor";
	}

	return false;
}

//==================================================================================================================//

bool InteraccionTelegram::evaluarMensajeFloat(float Avalor_min, float Avalor_max, String Aunidad)
{
	delay(10000); // esperamos 10 segundos para introducir una respuesta

	TBMessage msj_FB;
	// si llegó un mensaje
	if (CTBotMessageText == Bot.getNewMessage(msj_FB))
	{
		CT.numero_entrada_float = msj_FB.text.toFloat(); // pasamos el texto a número
		// si el número es válido
		if (CT.numero_entrada_float >= Avalor_min  &&  CT.numero_entrada_float <= Avalor_max)
		{
			CT.respuesta = "Se ingresó: ";
			CT.respuesta += String(CT.numero_entrada_float);
			CT.respuesta += " ";
			CT.respuesta += Aunidad;
			CT.respuesta += ".\n\nEl valor se cambió exitosamente";
			return true; // devuelve siempre false, excepto que el número sea perfecto
		}
		else
		{
			CT.respuesta = "El número ingresado no es válido. El valor no cambió.\n";
			CT.respuesta += "Intente revisar los límites máximos y/o mínimos del valor (si los hubiera)";
		}
	}
	else // si no llegó un mensaje
	{
		CT.respuesta = "No se introdujo un número. El valor no cambió.\n";
		CT.respuesta += "Cuando realiza un cambio, tiene 10 segundos para introducir su nuevo valor";
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
		if (CT.id == 0)
			return; // el usuario debe hablar al Bot primeramente
		String mensaje;
		mensaje = "ALARMA: ";

		// evaluamos la temperatura
		if (temp_interior_promedio >= temp_maxima_alarma)
		{
			mensaje += "La temperatura del invernadero es excesivamente alta";
			CT.enviarMensaje(CT.id, mensaje);
		}
		else if (temp_interior_promedio <= temp_minima_alarma)
		{
			mensaje += "La temperatura del invernadero es excesivamente baja";
			CT.enviarMensaje(CT.id, mensaje);
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
			CT.comandoAlarma();
// DEBUG ====================================================
//			imprimirln("Ahora en la dirección 7 (alarma_activada) hay un: ");
//			imprimirln(EEPROM.get(direccion[6], alarma_activada));
// DEBUG ====================================================
*/

#endif