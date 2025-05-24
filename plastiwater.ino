#include <WiFi.h>
#include <PubSubClient.h>

const char* ssid = "SEU_WIFI";
const char* password = "SENHA_WIFI";
const char* mqtt_server = "broker.hivemq.com";

WiFiClient espClient;
PubSubClient client(espClient);

int sensorPin = 34;
int ledR = 25;
int ledG = 26;
int ledB = 27;
int buzzer = 14;

void setup_wifi() {
  delay(10);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(sensorPin, INPUT);
  pinMode(ledR, OUTPUT);
  pinMode(ledG, OUTPUT);
  pinMode(ledB, OUTPUT);
  pinMode(buzzer, OUTPUT);
  
  setup_wifi();
  client.setServer(mqtt_server, 1883);
}

void loop() {
  if (!client.connected()) {
    client.connect("PlastiWaterClient");
  }
  client.loop();

  int sensorValue = analogRead(sensorPin);
  float ntu = sensorValue / 4095.0 * 3000.0;

  String payload = String(ntu);
  client.publish("plastiwater/agua", payload.c_str());

  // Classificação
  if (ntu < 100) {
    digitalWrite(ledG, HIGH); digitalWrite(ledR, LOW); digitalWrite(ledB, LOW);
    digitalWrite(buzzer, LOW);
  } else if (ntu < 500) {
    digitalWrite(ledG, LOW); digitalWrite(ledR, LOW); digitalWrite(ledB, HIGH);
    digitalWrite(buzzer, LOW);
  } else {
    digitalWrite(ledG, LOW); digitalWrite(ledR, HIGH); digitalWrite(ledB, LOW);
    digitalWrite(buzzer, HIGH);
  }

  delay(5000);
}
