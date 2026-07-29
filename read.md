# 🌿 Estufa Agrícola Automatizada com ESP32 e IoT

![Status](https://img.shields.io/badge/Status-Conclu%C3%ADdo-brightgreen)
![Microcontrolador](https://img.shields.io/badge/Hardware-ESP32-blue)
![Protocolo](https://img.shields.io/badge/IoT-MQTT%20%2F%20JSON-orange)

## 📌 Proposta e Objetivos
Este projeto consiste no desenvolvimento de um protótipo funcional de uma **Estufa Agrícola Automatizada integrada à Internet das Coisas (IoT)**. O sistema monitora variáveis ambientais vitais (Temperatura, Umidade do Ar, Luminosidade e Nível do Solo/Reservatório) para tomar decisões de atuamento em tempo real (ativação de irrigação, exaustão e alarmes), além de transmitir a telemetria via rede Wi-Fi.

---

## ⚠️ Cuidados Técnicos de Segurança Elétrica (Atenção Máxima)
A placa **ESP32 opera com nível lógico estrito de 3,3 V**. A injeção de 5 V diretamente nas GPIOs danifica a porta do microcontrolador.

* **Potenciômetro e LDR:** Alimentados obrigatoriamente na linha de **3,3 V**.
* **Display LCD 16x2 I2C:** Alimentado no pino **VIN / 5V** para manter o contraste e brilho adequado, aproveitando a tolerância dos pinos I2C (GPIO 21 e 22).
* **LEDs:** Todos acompanhados de resistor limitador de corrente de **220 Ω** em série.
* **Chaveamento por Transistor (NPN):** A base do transistor conta com resistor limitador em série (**1 kΩ** a **10 kΩ**).

---

## 🔌 Mapeamento de Pinos e Esquema Elétrico (GPIOs)

| Componente | Alimentação (VCC) | Terra (GND) | Pino Sinal / Função | Categoria / Detalhe |
| :--- | :--- | :--- | :--- | :--- |
| **ESP32 DevKit** | USB (5 V) | GND | Microcontrolador Central | Processamento |
| **Display LCD 16x2 I2C** | 5 V (VIN) | GND | **GPIO 21** (SDA) / **GPIO 22** (SCL) | Exibição Local |
| **Sensor DHT22** | 3,3 V | GND | **GPIO 4** (Entrada Digital) | Temp. e Umidade Ar |
| **Módulo LDR** | 3,3 V *(Obrigatório)* | GND | **GPIO 34** (ADC1 Analógico) | Luminosidade |
| **Potenciômetro 10 kΩ** | 3,3 V *(Obrigatório)* | GND | **GPIO 35** (ADC1 Analógico) | Nível Reservatório / Solo |
| **LED (Irrigação)** | - | GND (Resistor 220 Ω) | **GPIO 18** (Saída Digital) | Bomba de Irrigação |
| **Buzzer 5V** | - | GND | **GPIO 19** (Saída Digital) | Alarme de Temperatura |
| **Transistor NPN (Cooler)**| 5 V (VIN) | GND | **GPIO 25** (Resistor 1 kΩ na Base)| Saída PWM (Exaustão) |

---

## ⚡ Lógica de Automação do Sistema
1. **Irrigação Automática:** O acionamento do LED de irrigação ocorre quando o nível do reservatório/solo estiver crítico (< 30%).
2. **Alarme Crítico de Exaustão:** Disparo contínuo do Buzzer caso a temperatura ultrapasse **30,0 °C**.
3. **Telemetria IoT:** Envio dos dados estruturados em JSON via protocolo **MQTT** a cada 2 segundos.

---

## 💡 Diferencial Técnico e Inovação da Equipe
Para superar os requisitos mínimos do projeto, a equipe implementou dois recursos adicionais:
1. **Controle de Exaustão com PWM Proporcional:** Utilização de um transistor NPN acionado via canal PWM na GPIO 25 da ESP32. A rotação da ventoinha cresce gradualmente quando a temperatura varia de **28 °C a 35 °C**, otimizando o consumo energético e reduzindo ruído em comparação a um acionamento chaveado simples.
2. **Interlock Antimofo (Cruzamento de Dados):** O sistema suspende a irrigação do solo se a umidade relativa do ar for superior a **85%**, evitando o encharcamento da estufa e a proliferação de fungos patogênicos.

---

## 📦 Lista de Materiais (BOM)
* 1x Placa ESP32 DevKit V1
* 1x Protoboard de 830 pontos
* 1x Display LCD 16x2 com Módulo I2C
* 1x Sensor de Temperatura e Umidade DHT22 (ou DHT11)
* 1x Módulo Sensor LDR
* 1x Potenciômetro Linear de 10 kΩ
* 1x LED Difuso 5mm
* 1x Buzzer Ativo 5V
* 1x Transistor NPN (Ex: BC547 ou BC337)
* Resistores: 1x 220 Ω e 1x 1 kΩ
* Jumper Macho-Macho / Macho-Fêmea


## 👥 Integrantes da Equipe
* **[Ana Luiza]** - *Montagem do Circuito e Hardware*
* **[Nali ]** - *Desenvolvimento do Código C++ (ESP32)*
* **[Alice ]** - *Integração IoT e Frontend / Dashboard*