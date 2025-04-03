#include <WiFi.h>
#include "src.h"
#include <ModbusIP_ESP8266.h>

// Configuración de red
//const char* ssid = "Mega_2.4G_E4A1";const char* password = "qHkqSqKc";
const char* ssid = "Mega-2.4G-16B1"; const char* password = "Jb8w5mzByz";


WiFiServer server(80);

// Configuración de Modbus
const int REG = 0;
IPAddress remote(192, 168, 1, 3);
ModbusIP mb;

// Parámetros globales
const int LOOP_COUNT = 10;
uint16_t res = 0;
uint8_t show = LOOP_COUNT;

// Función para obtener el registro correspondiente a cada bobina
int registro(String bobina) {
    if (bobina == "A+") return 1;
    if (bobina == "A-") return 2;
    if (bobina == "B+") return 4;
    if (bobina == "B-") return 8;
    if (bobina == "C+") return 16;
    if (bobina == "C-") return 32;
    if (bobina == "D+") return 64;
    if (bobina == "D-") return 128;
    return 0;
}

// Función para leer el sensor asociado a una bobina
String lecturaSensor(String bobina) {
    int Hreg;
    String sensor;
    if (bobina == "A+") {
      sensor = "a1";
      Hreg = 2;
      }
    else if (bobina == "A-") {
      sensor = "a0";
      Hreg = 1;
    }
    else if (bobina == "B+") {
      sensor = "b1";
      Hreg = 4;
    }
    else if (bobina == "B-") {
      sensor = "b0";
      Hreg = 3;
    }
    else if (bobina == "C+") {
      sensor = "c1";
      Hreg = 6;
    }
    else if (bobina == "C-") {
      sensor = "c0";
      Hreg = 5;
    }
    else if (bobina == "D+") {
      sensor = "d1";
      Hreg = 8;
    }
    else if (bobina == "D-") {
      sensor = "d0";
      Hreg = 7;
    }
    else return "0";
    mb.task();
    uint16_t respuesta;
    mb.readHreg(remote, Hreg, &respuesta);
    //Serial.print("Sensor: ");
    //Serial.print(sensor);
    //Serial.print("\t");
    String salida = bobina + String(respuesta);
    //Serial.print("\t Respuesta recibida: ");
    //Serial.print(salida);
    delay(10);
    return salida;
}

// Función para dividir una cadena en elementos usando un delimitador
void splitString(String input, char delimiter, String output[], int &size) {
    int inicio = 0, fin = 0, indice = 0;
    while ((fin = input.indexOf(delimiter, inicio)) != -1) {
        output[indice++] = input.substring(inicio, fin);
        inicio = fin + 1;
    }
    output[indice++] = input.substring(inicio);
    size = indice;
}

// Configuración inicial
void setup() {
    Serial.begin(115200);
    
    WiFi.begin(ssid, password);
    Serial.print("Conectando a WiFi...");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nConectado a WiFi");
    Serial.print("Dirección IP: ");
    Serial.println(WiFi.localIP());
    
    server.begin();
    mb.client();
    mb.disconnect(remote);    
}

// Bucle principal
void loop() {
    WiFiClient client = server.available();
    if (client) {
        Serial.println("Cliente conectado");
        String request = "";
        while (client.connected()) {
            if (client.available()) {
                char c = client.read();
                request += c;
                if (c == '\n' && request.length() > 1) {
                    break;
                }
            }
        }
        
        if (request.indexOf("GET /?secuencia=") != -1) {
            int startIndex = request.indexOf("secuencia=") + 10;
            int endIndex = request.indexOf(" ", startIndex);
            String secuencia = request.substring(startIndex, endIndex);
            secuencia.replace("%20", " ");
            secuencia.replace("%2B", "+"); 
            secuencia.replace(" ", ","); 
            Serial.println("Secuencia recibida: " + secuencia);
            
            const int maxAcciones = 50;
            String bobinas[maxAcciones];
            int size = 0;
            splitString(secuencia, ',', bobinas, size);
            Serial.print("Tamaño de la secuencia: ");
            Serial.println(size);
            for (int i = 0; i < size; i++) {
                if (registro(bobinas[i]) > 0) {
                    Serial.print("\nBobina ");
                    Serial.println(bobinas[i]);
                    //Serial.print(":");
                    //Serial.println(registro(bobinas[i]));
                    mb.writeHreg(remote, REG, registro(bobinas[i]));
                    mb.task();
                    lecturaSensor(bobinas[i]);
                    lecturaSensor(bobinas[i]);
                    lecturaSensor(bobinas[i]);
                    lecturaSensor(bobinas[i]);
                    lecturaSensor(bobinas[i]);
                    lecturaSensor(bobinas[i]);
                    String respuestaEsperada = bobinas[i] + "1";
                    String respuestaRecibida = lecturaSensor(bobinas[i]);
                    for (int j = 0; j < 30; j++) {
                      respuestaRecibida = lecturaSensor(bobinas[i]);
                      delay(5);
                    }
                    Serial.print("Respuesta esperada: ");
                    Serial.print(respuestaEsperada);
                    Serial.print("\t Respuesta recibida: ");
                    Serial.print(respuestaRecibida);
                    Serial.println("\t Desde afuera");
                    while (respuestaRecibida != respuestaEsperada) {
                      respuestaRecibida = lecturaSensor(bobinas[i]);
                      Serial.print("Respuesta esperada: ");
                      Serial.print(respuestaEsperada);
                      Serial.print("\t Respuesta recibida: ");
                      Serial.print(respuestaRecibida);
                      Serial.println();
                    }
                    Serial.println("Salir del bucle.");

                }
            }
        }
        
        client.println("HTTP/1.1 200 OK");
        client.println("Content-Type: text/html");
        client.println("Connection: close");
        client.println();
        client.println(html);
        
        client.stop();
        Serial.println("Cliente desconectado");
    }
    
    if (mb.isConnected(remote)) {
        //mb.readHreg(remote, REG, &res);
    } else {
        mb.connect(remote);
        Serial.println("Intentando conectar...");
    }
    mb.task();
    delay(100);
    
    if (!show--) {
        Serial.println(res);
        show = LOOP_COUNT;
    }
}