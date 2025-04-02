#include <WiFi.h>
#include "src.h"
#include <ModbusIP_ESP8266.h>

const int REG = 0;               // Modbus Hreg Offset
IPAddress remote(192, 168, 100, 18);  // Address of Modbus Slave device
const int LOOP_COUNT = 10;

ModbusIP mb;  //ModbusIP object

const char* ssid = "Mega-2.4G-16B1";
const char* password = "Jb8w5mzByz";
WiFiServer server(80);

int registro(String bobina) {
    int valor;
    if(bobina == "A+"){
      valor = 1;
    }else if(bobina == "A-"){
      valor = 2;
    }else if(bobina == "B+"){
      valor = 4;
    }else if(bobina == "B-"){
      valor = 8;
    }else if(bobina == "C+"){
      valor = 16;
    }else if(bobina == "C-"){
      valor = 32;
    }else if(bobina == "D+"){
      valor = 64;
    }else if(bobina == "D-"){
      valor = 128;
    }
    else{
      valor = 0;
    }
    return valor;
}

int lecturaSensor(String bobina){
  int Hreg;
  uint16_t respuesta;
    if(bobina == "A+"){
      Hreg = 1;
    }else if(bobina == "A-"){
      Hreg = 2;
    }else if(bobina == "B+"){
      Hreg = 3;
    }else if(bobina == "B-"){
      Hreg = 4;
    }else if(bobina == "C+"){
      Hreg = 5;
    }else if(bobina == "C-"){
      Hreg = 6;
    }else if(bobina == "D+"){
      Hreg = 7;
    }else if(bobina == "D-"){
      Hreg = 8;
    }
    else{
      Hreg = 49;
    }
    mb.readHreg(remote, Hreg, &respuesta);
    mb.task();
    return respuesta;
}

void splitString(String input, char delimiter, String output[], int size = 0) {
    int inicio = 0, fin = 0, indice = 0;
    while ((fin = input.indexOf(delimiter, inicio)) != -1) {
        output[indice++] = input.substring(inicio, fin);
        inicio = fin + 1;
    }
    
    output[indice++] = input.substring(inicio); // Último fragmento
    size = indice; // Devolver la cantidad de elementos almacenados
}

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
uint16_t res = 0;
uint8_t show = LOOP_COUNT;

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
        //Serial.println(request);
        if (request.indexOf("GET /?secuencia=") != -1) {
            int startIndex = request.indexOf("secuencia=") + 10;
            int endIndex = request.indexOf(" ", startIndex);
            String secuencia = request.substring(startIndex, endIndex);
            secuencia.replace("%20", " ");// Reemplazar espacios codificados en URL
            secuencia.replace("%2B", "+"); 
            secuencia.replace(" ", ","); 
            Serial.println("Secuencia recibida: " + secuencia);
            const int maxAcciones = 50;
            String secuencias[maxAcciones];
            splitString(secuencia, ',',secuencias);
            for(int i = 0; i < maxAcciones; i++){
              if(registro(secuencias[i])>0){
                Serial.print("Registro: ");
                Serial.print(secuencias[i]);
                Serial.print(":");
                Serial.println(registro(secuencias[i]));
                mb.writeHreg(remote, REG, registro(secuencias[i]));
                mb.task();
                while(lecturaSensor(secuencias[i]) != 1){

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
    if (mb.isConnected(remote)) {   // Check if connection to Modbus Slave is established
      //Serial.println("Connected");
      mb.readHreg(remote, REG, &res);  // Initiate Read Coil from Modbus Slave
    } else {
      mb.connect(remote);           // Try to connect if no connection
      Serial.println("Trying to connect.");
    }
    mb.task();                      // Common local Modbus task
    delay(100);                     // Pulling interval
    if (!show--) {                   // Display Slave register value one time per second (with default settings)
      Serial.println(res);
      show = LOOP_COUNT;
    }

}
