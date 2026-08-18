#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>

// ==========================================
// MAPEAMENTO DE HARDWARE (ESP32-S3)
// ==========================================
#define DHTPIN        15      // Entrada de dados do sensor de clima
#define DHTTYPE       DHT11   // Modelo do sensor (DHT11 ou DHT22)
#define LDR_PIN       4       // Leitura analógica de luminosidade (ADC1)
#define POT_PIN       5       // Leitura analógica de umidade do solo (ADC1)
#define LED_IRRIGACAO 6       // Sinal de acionamento do sistema de rega
#define BUZZER_PIN    7       // Sinal de acionamento do alarme sonoro

// Configuração dos barramentos de comunicação
#define I2C_SDA       8       // Linha de dados I2C do display
#define I2C_SCL       9       // Linha de clock I2C do display

// Limites operacionais para automação
#define TEMP_LIMITE_ALTA 30.0 // Gatilho de temperatura crítica em °C
#define SOLO_CRITICO     300  // Limiar mínimo de umidade do solo (valor ADC)

// Instanciamento dos periféricos
LiquidCrystal_I2C lcd(0x27, 16, 2);
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  Serial.println("\n--- Sistema de Monitoramento de Estufa ESP32-S3 ---");

  // Configura os atuadores como saídas e garante estado desligado
  pinMode(LED_IRRIGACAO, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(LED_IRRIGACAO, LOW);
  digitalWrite(BUZZER_PIN, LOW);

  // Inicializa o protocolo I2C com os pinos redefinidos
  Wire.begin(I2C_SDA, I2C_SCL);

  // Prepara a tela LCD e exibe mensagem de boot
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Estufa ESP32-S3");
  lcd.setCursor(0, 1);
  lcd.print("Iniciando...");

  // Ativa a coleta do sensor de umidade/temperatura
  dht.begin();
  delay(2000);
  lcd.clear();
}

void loop() {
  // Captura dos dados dos sensores
  float temperatura = dht.readTemperature();
  float umidade = dht.readHumidity();
  int leituraLDR = analogRead(LDR_PIN);
  int leituraSolo = analogRead(POT_PIN);

  // Validação das leituras do DHT
  if (isnan(temperatura) || isnan(umidade)) {
    Serial.println("Falha na comunicação com o sensor DHT!");
    lcd.setCursor(0, 0);
    lcd.print("Erro no DHT!    ");
    delay(2000);
    return;
  }

  // Converte a leitura do solo para porcentagem (0 a 100%)
  int pctSolo = map(leituraSolo, 0, 4095, 0, 100);

  // Regra de acionamento da irrigação
  if (leituraSolo < SOLO_CRITICO) {
    digitalWrite(LED_IRRIGACAO, HIGH); // Ativa bomba/solenoide
  } else {
    digitalWrite(LED_IRRIGACAO, LOW);  // Desativa irrigação
  }

  // Regra de acionamento do alarme de superaquecimento
  if (temperatura > TEMP_LIMITE_ALTA) {
    digitalWrite(BUZZER_PIN, HIGH);   // Dispara o alerta sonoro
  } else {
    digitalWrite(BUZZER_PIN, LOW);    // Mantém em silêncio
  }

  // Atualiza as informações da Linha 1 no LCD (Temperatura e Umidade)
  lcd.setCursor(0, 0);
  lcd.print("T:");
  lcd.print(temperatura, 1);
  lcd.print((char)223);
  lcd.print("C U:");
  lcd.print(umidade, 0);
  lcd.print("%   ");

  // Atualiza as informações da Linha 2 no LCD (Umidade do Solo e Luz)
  lcd.setCursor(0, 1);
  lcd.print("Solo:");
  lcd.print(pctSolo);
  lcd.print("% L:");
  lcd.print(map(leituraLDR, 0, 4095, 0, 100));
  lcd.print("%   ");

  // Transmita as leituras formatadas via porta Serial
  Serial.printf("Temp: %.1f°C | Umid: %.1f%% | Solo: %d%% | Luz ADC: %d\n",
                temperatura, umidade, pctSolo, leituraLDR);

  // Intervalo entre cada amostragem
  delay(2000);
}
