#include <TinyGPSPlus.h>
#include <WiFi.h>
#include <HTTPClient.h>

#define GPS_BAUDRATE 9600  // The default baudrate of NEO-6M is 9600

TinyGPSPlus gps;  // the TinyGPS++ object

double latitude = 0.0;
double longitude = 0.0;
double altitude = 0.0;
double speed = 0.0;

const char* ssid = "ComputadorThiago";
const char* password = "thiago06102006";
const char* apiEndpoint = "http://192.168.1.6:5000/api/infos/sensores";

void setup() {
  Serial.begin(9600);
  Serial2.begin(GPS_BAUDRATE); // Serial2 for GPS UART

  Serial.println(F("ESP32 - GPS module_Simulation"));

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi conectado!");
  Serial.println("IP local: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  while (Serial2.available() > 0) {
    gps.encode(Serial2.read());
  }

  if (gps.location.isUpdated()) {
    latitude = gps.location.lat();
    longitude = gps.location.lng();
    altitude = gps.altitude.isValid() ? gps.altitude.meters() : 0.0;
    speed = gps.speed.isValid() ? gps.speed.kmph() : 0.0;

    Serial.print("latitude: ");
    Serial.println(latitude, 6);
    Serial.print("longitude: ");
    Serial.println(longitude, 6);
    Serial.print("altitude: ");
    Serial.println(altitude, 2);
    Serial.print("velocidade: ");
    Serial.println(speed, 2);

    // Criação da string JSON com os dados de temperatura e umidade
    String dadosParaEnviar = "{\"longitude\": ";
      dadosParaEnviar += String(longitude, 6);
      dadosParaEnviar += ", \"latitude\": ";
      dadosParaEnviar += String(latitude, 6);
      dadosParaEnviar += ", \"altitude\": ";
      dadosParaEnviar += String(altitude, 2);
      dadosParaEnviar += ", \"speed\": ";
      dadosParaEnviar += String(speed, 2);
      dadosParaEnviar += "}";

    // Exibe os dados JSON que serão enviados para depuração
    Serial.print("Dados JSON para enviar: ");
    Serial.println(dadosParaEnviar);

    // Inicia a comunicação HTTP
    HTTPClient http;
    http.begin(apiEndpoint);
    http.addHeader("Content-Type", "application/json"); 
    int codigoRespostaHttp = http.POST(dadosParaEnviar);

    // Verifica a resposta HTTP
    if (codigoRespostaHttp > 0) {
        Serial.print("Código de resposta Http: ");
        Serial.println(codigoRespostaHttp);

        if (codigoRespostaHttp == HTTP_CODE_OK || codigoRespostaHttp == HTTP_CODE_CREATED) {
            String resposta = http.getString();
            Serial.println("Resposta da API:");
            Serial.println(resposta);
        } else {
            Serial.println("Falha ao enviar dados");
            Serial.println(codigoRespostaHttp);
        }
    } else {
        Serial.println("Erro ao enviar requisição HTTP");
    }

    // Encerra a comunicação HTTP e aguarda um intervalo
    http.end();
  } else {
    Serial.println("Localização ainda não atualizada ou inválida");
  }

  delay(1000);
}
