#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <stdlib.h>

const char *ssid = "AP-204";
const char *password = "999094047";

const char *BROKER_MQTT = "test.mosquitto.org";
int BROKER_PORT = 1883;
#define ID_MQTT "user_prova_iot"
#define TOPIC_01 "cesul_prova_de_iot/temperatura"

WiFiClient espClient;
PubSubClient MQTT(espClient);

void setup() {
  Serial.begin(115200);
  WiFi.hostname("nodemcu_prova_iot");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Conectado ao WiFi!");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());

  MQTT.setServer(BROKER_MQTT, BROKER_PORT);
  conectaMQTT();
}

void loop() {
  static long long temporizador_envio = 0;
  mantemConexoes();

  if (millis() > temporizador_envio + 1000) {
    enviaValores();
    temporizador_envio = millis();
  }

  MQTT.loop();
}

void conectaMQTT() {
  while (!MQTT.connected()) {
    Serial.print("Conectando ao Broker MQTT: ");
    Serial.println(BROKER_MQTT);

    if (MQTT.connect(ID_MQTT)) {
      Serial.println("Conectado ao Broker com sucesso!");
    } else {
      Serial.print("Falha na conexão, rc=");
      Serial.print(MQTT.state());
      Serial.println(" Tentando novamente...");
      delay(1000);
    }
  }
}

void enviaValores() {
  char valor[4];
  itoa(random(0, 101), valor, 10);
  MQTT.publish(TOPIC_01, valor);
}

void mantemConexoes() {
  if (!MQTT.connected()) {
    conectaMQTT();
  }
}
