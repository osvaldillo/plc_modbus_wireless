#include <WiFi.h>
#include "src.h"
#include <ModbusIP_ESP8266.h>

// Configuración de red
const char* ssid = "Mega-2.4G-16B1";
const char* password = "Jb8w5mzByz";
WiFiServer server(80);

// Configuración de Modbus
const int REG = 0;
IPAddress remote(192, 168, 100, 18);
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
int lecturaSensor(String bobina) {
    int Hreg;
    if (bobina == "A+") Hreg = 1;
    else if (bobina == "A-") Hreg = 2;
    else if (bobina == "B+") Hreg = 3;
    else if (bobina == "B-") Hreg = 4;
    else if (bobina == "C+") Hreg = 5;
    else if (bobina == "C-") Hreg = 6;
    else if (bobina == "D+") Hreg = 7;
    else if (bobina == "D-") Hreg = 8;
    else Hreg = 49;
    
    uint16_t respuesta;
    mb.readHreg(remote, Hreg, &respuesta);
    mb.task();
    return respuesta;
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
            String secuencias[maxAcciones];
            int size = 0;
            splitString(secuencia, ',', secuencias, size);
            
            for (int i = 0; i < size; i++) {
                if (registro(secuencias[i]) > 0) {
                    Serial.print("Registro: ");
                    Serial.print(secuencias[i]);
                    Serial.print(":");
                    Serial.println(registro(secuencias[i]));
                    mb.writeHreg(remote, REG, registro(secuencias[i]));
                    mb.task();
                    while (lecturaSensor(secuencias[i]) != 1) {
                        // Esperar a que la lectura del sensor sea 1
                    }
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
        mb.readHreg(remote, REG, &res);
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