# PlastiWater

Projeto fictício de IoT para detecção de microplásticos usando ESP32, sensor de turbidez e protocolo MQTT.

## Componentes utilizados
- ESP32 DevKit v1
- Sensor de Turbidez DFRobot SEN0189
- LED RGB
- Buzzer
- Jumpers, resistores e protoboard

## Funcionamento
1. O sensor mede turbidez simulada.
2. ESP32 classifica e aciona LED/buzzer.
3. Dados são enviados via MQTT para o broker HiveMQ.
4. Visualização via MQTT Explorer.

## Códigos
- Código principal: `src/main.cpp`
- Configuração PlatformIO: `platformio.ini`

## Comunicação
- Broker: HiveMQ (broker.hivemq.com)
- Tópico: `plastiwater/agua`
- Protocolo: MQTT (porta 1883)

## Vídeo de Demonstração
[Assista aqui](https://youtu.be/SEU-LINK-AQUI)

## Estrutura
