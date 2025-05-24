#include <WiFi.h>
#include <PubSubClient.h>

const char* ssid = "SEU_WIFI";       // Substitua pelo nome da sua rede Wi-Fi
const char* password = "SENHA_WIFI"; // Substitua pela senha da sua rede Wi-Fi
const char* mqtt_server = "broker.hivemq.com"; // Broker MQTT público

WiFiClient espClient;
PubSubClient client(espClient);

// Definição dos pinos do ESP32
int sensorPin = 34; // Pino analógico para o sensor de turbidez
int ledR = 25;      // Pino para o componente Vermelho do LED RGB
int ledG = 26;      // Pino para o componente Verde do LED RGB
int ledB = 27;      // Pino para o componente Azul do LED RGB
int buzzer = 14;    // Pino para o Buzzer

// Função para conectar ao Wi-Fi
void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Conectando-se a ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi conectado!");
  Serial.print("Endereço IP: ");
  Serial.println(WiFi.localIP());
}

// Função de callback para mensagens MQTT (não usada para este projeto de publicador simples, mas boa prática)
void callback(char* topic, byte* payload, unsigned int length) {
  // Serial.print("Mensagem recebida [");
  // Serial.print(topic);
  // Serial.print("] ");
  // for (int i = 0; i < length; i++) {
  //   Serial.print((char)payload[i]);
  // }
  // Serial.println();
}

// Função para reconectar ao broker MQTT
void reconnect_mqtt() {
  while (!client.connected()) {
    Serial.print("Tentando conectar ao broker MQTT...");
    // Crie um ID de cliente MQTT aleatório
    String clientId = "PlastiWaterClient-";
    clientId += String(random(0xffff), HEX);
    // Tenta conectar
    if (client.connect(clientId.c_str())) {
      Serial.println("conectado");
      // Opcional: subscrever a um tópico se o dispositivo precisasse receber comandos
      // client.subscribe("plastiwater/comandos");
    } else {
      Serial.print("falha, rc=");
      Serial.print(client.state());
      Serial.println(" tentando novamente em 5 segundos");
      // Espera 5 segundos antes de tentar novamente
      delay(5000);
    }
  }
}

// Função de configuração inicial
void setup() {
  Serial.begin(115200); // Inicializa comunicação serial para debug

  // Configura os pinos dos atuadores como saída
  pinMode(sensorPin, INPUT);
  pinMode(ledR, OUTPUT);
  pinMode(ledG, OUTPUT);
  pinMode(ledB, OUTPUT);
  pinMode(buzzer, OUTPUT);
  
  // Inicializa o estado dos LEDs e Buzzer
  digitalWrite(ledR, LOW);
  digitalWrite(ledG, LOW);
  digitalWrite(ledB, LOW);
  digitalWrite(buzzer, LOW);

  setup_wifi(); // Conecta ao Wi-Fi
  client.setServer(mqtt_server, 1883); // Configura o servidor MQTT e porta
  client.setCallback(callback); // Define a função de callback para mensagens recebidas (se houver)
}

// Loop principal do programa
void loop() {
  // Garante que o cliente MQTT está conectado. Se não, tenta reconectar.
  if (!client.connected()) {
    reconnect_mqtt();
  }
  client.loop(); // Processa mensagens MQTT pendentes e mantém a conexão

  // --- Simulação da leitura do sensor de turbidez ---
  // Gerar um valor aleatório dentro da faixa de leitura do ADC (0 a 4095 para ESP32)
  // Isso simula a variação de poluição ao longo do tempo
  int simulatedSensorRawValue = random(0, 4095); // Simula valores brutos do sensor
  
  // Converte o valor bruto simulado para NTU (Unidade Nefelométrica de Turbidez)
  // Assumindo que 4095 (valor máximo) = 3000 NTU (limite superior do sensor DFRobot SEN0189)
  float ntu = (float)simulatedSensorRawValue / 4095.0 * 3000.0;

  // --- Publicação dos dados via MQTT ---
  String payload = "{\"turbidez\": " + String(ntu) + "}"; // Formato JSON para o payload
  client.publish("plastiwater/agua", payload.c_str()); // Publica no tópico "plastiwater/agua"
  Serial.print("Dados de turbidez simulados enviados: ");
  Serial.print(ntu);
  Serial.println(" NTU");

  // --- Classificação e Acionamento dos Atuadores ---
  // Baseado nos níveis de NTU para acionar LED RGB e Buzzer
  if (ntu < 100) {
    // Água Limpa: LED Verde
    digitalWrite(ledR, LOW);
    digitalWrite(ledG, HIGH);
    digitalWrite(ledB, LOW);
    digitalWrite(buzzer, LOW); // Buzzer desligado
    Serial.println("Status: Água Limpa (LED Verde)");
  } else if (ntu >= 100 && ntu <= 500) { // Faixa de turbidez moderada
    // Poluição Moderada: LED Amarelo (Vermelho + Verde = Amarelo)
    digitalWrite(ledR, HIGH);
    digitalWrite(ledG, HIGH);
    digitalWrite(ledB, LOW);
    digitalWrite(buzzer, LOW); // Buzzer desligado
    Serial.println("Status: Alerta Moderado (LED Amarelo)");
  } else { // ntu > 500
    // Poluição Crítica: LED Vermelho + Buzzer
    digitalWrite(ledR, HIGH);
    digitalWrite(ledG, LOW);
    digitalWrite(ledB, LOW);
    digitalWrite(buzzer, HIGH); // Buzzer ligado para alarme crítico
    Serial.println("Status: Alerta Crítico (LED Vermelho e Buzzer)");
  }

  delay(5000); // Aguarda 5 segundos antes da próxima leitura/ciclo
}