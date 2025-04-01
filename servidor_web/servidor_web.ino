#include <WiFi.h>
#include "src.h"

const char* ssid = "Mega-2.4G-16B1";
const char* password = "Jb8w5mzByz";
WiFiServer server(80);

const int ledPin = 2;

void setup() {
    Serial.begin(115200);
    pinMode(ledPin, OUTPUT);
    
    WiFi.begin(ssid, password);
    Serial.print("Conectando a WiFi");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    
    Serial.println("\nConectado a WiFi");
    Serial.print("Dirección IP: ");
    Serial.println(WiFi.localIP());
    
    server.begin();
}

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
        
        Serial.println(request);
        if (request.indexOf("GET /?led=on") != -1) {
            digitalWrite(ledPin, HIGH);
        } else if (request.indexOf("GET /?led=off") != -1) {
            digitalWrite(ledPin, LOW);
        }
        
        //String html = generateHTML();
        
        client.println("HTTP/1.1 200 OK");
        client.println("Content-Type: text/html");
        client.println("Connection: close");
        client.println();
        client.println(html);
        
        client.stop();
        Serial.println("Cliente desconectado");
    }
}
