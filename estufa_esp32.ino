#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>
#include <WiFi.h>
#include <WebServer.h>

// ==========================================
// CONFIGURAÇÕES DE REDE (WI-FI)
// ==========================================
const char* ssid = "NOME_DA_SUA_REDE";
const char* password = "SENHA_DA_SUA_REDE";

WebServer server(80);

// ==========================================
// MAPEAMENTO DE HARDWARE (ESP32-S3)
// ==========================================
#define DHTPIN        15      // Entrada de dados do sensor de clima
#define DHTTYPE       DHT11   // Modelo do sensor (DHT11 ou DHT22)
#define LDR_PIN       4       // Leitura analógica de luminosidade (ADC1)
#define POT_PIN       5       // Leitura analógica de umidade do solo (ADC1)

#define LED_IRRIGACAO 6       // Pino da Bomba de Irrigação
#define BUZZER_PIN    7       // Sinal de acionamento do alarme sonoro
#define COOLER_PIN    10      // Pino do Cooler/Exaustão
#define LUZ_PIN       11      // Pino da Iluminação Grow Light

// Configuração dos barramentos de comunicação
#define I2C_SDA       8       // Linha de dados I2C do display
#define I2C_SCL       9       // Linha de clock I2C do display

// Limites operacionais para automação
#define TEMP_LIMITE_ALTA 30.0 // Gatilho de temperatura crítica em °C
#define SOLO_CRITICO     300  // Limiar mínimo de umidade do solo (valor ADC)

// Instanciamento dos periféricos
LiquidCrystal_I2C lcd(0x27, 16, 2);
DHT dht(DHTPIN, DHTTYPE);

// Variáveis globais
float temperatura = 0.0;
float umidade = 0.0;
int leituraLDR = 0;
int leituraSolo = 0;
int pctSolo = 0;
int pctLuz = 0;

bool bombaManual = false;
bool coolerManual = false;
bool luzManual = false;

// Configuração do CORS para permitir requisições de páginas web externas
void setCORS() {
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.sendHeader("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
  server.sendHeader("Access-Control-Allow-Headers", "Content-Type");
}

// Endpoint JSON para enviar TODOS os dados do ESP32 para o site
void handleData() {
  setCORS();
  String json = "{";
  json += "\"temp\":" + String(temperatura) + ",";
  json += "\"umidade\":" + String(umidade) + ",";
  json += "\"luz\":" + String(pctLuz) + ",";
  json += "\"soloPct\":" + String(pctSolo) + ",";
  json += "\"bombaState\":" + String(digitalRead(LED_IRRIGACAO) == HIGH ? "true" : "false") + ",";
  json += "\"coolerState\":" + String(digitalRead(COOLER_PIN) == HIGH ? "true" : "false") + ",";
  json += "\"luzState\":" + String(digitalRead(LUZ_PIN) == HIGH ? "true" : "false");
  json += "}";
  
  server.send(200, "application/json", json);
}

// Endpoint para controle individual de cada atuador
void handleToggle() {
  setCORS();
  if (server.hasArg("type") && server.hasArg("state")) {
    String type = server.arg("type");
    bool state = server.arg("state") == "1";

    if (type == "bomba") {
      bombaManual = state;
      digitalWrite(LED_IRRIGACAO, state ? HIGH : LOW);
    } else if (type == "cooler") {
      coolerManual = state;
      digitalWrite(COOLER_PIN, state ? HIGH : LOW);
    } else if (type == "luz") {
      luzManual = state;
      digitalWrite(LUZ_PIN, state ? HIGH : LOW);
    }
  }
  server.send(200, "text/plain", "OK");
}

void setup() {
  Serial.begin(115200);

  pinMode(LED_IRRIGACAO, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(COOLER_PIN, OUTPUT);
  pinMode(LUZ_PIN, OUTPUT);

  digitalWrite(LED_IRRIGACAO, LOW);
  digitalWrite(BUZZER_PIN, LOW);
  digitalWrite(COOLER_PIN, LOW);
  digitalWrite(LUZ_PIN, LOW);

  Wire.begin(I2C_SDA, I2C_SCL);

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Conectando WiFi");

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWi-Fi Conectado!");
  Serial.print("Endereço IP do ESP32: ");
  Serial.println(WiFi.localIP());

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("IP da Estufa:");
  lcd.setCursor(0, 1);
  lcd.print(WiFi.localIP());

  dht.begin();

  server.on("/data", handleData);
  server.on("/toggle", handleToggle);
  
  server.onNotFound([]() {
    if (server.method() == HTTP_OPTIONS) {
      setCORS();
      server.send(204);
    } else {
      server.send(404, "text/plain", "Not Found");
    }
  });

  server.begin();
  delay(3000);
  lcd.clear();
}

void loop() {
  server.handleClient();

  // Leitura dos sensores
  temperatura = dht.readTemperature();
  umidade = dht.readHumidity();
  leituraLDR = analogRead(LDR_PIN);
  leituraSolo = analogRead(POT_PIN);

  if (isnan(temperatura) || isnan(umidade)) {
    temperatura = 0.0;
    umidade = 0.0;
  }

  pctSolo = map(leituraSolo, 0, 4095, 0, 100);
  pctLuz = map(leituraLDR, 0, 4095, 0, 100);

  // Automação da bomba de irrigação (se não estiver sob controle manual)
  if (!bombaManual) {
    if (leituraSolo < SOLO_CRITICO) {
      digitalWrite(LED_IRRIGACAO, HIGH);
    } else {
      digitalWrite(LED_IRRIGACAO, LOW);
    }
  }

  // Automação de alarme sonoro
  if (temperatura > TEMP_LIMITE_ALTA) {
    digitalWrite(BUZZER_PIN, HIGH);
  } else {
    digitalWrite(BUZZER_PIN, LOW);
  }

  // Atualização LCD
  lcd.setCursor(0, 0);
  lcd.print("T:");
  lcd.print(temperatura, 1);
  lcd.print((char)223);
  lcd.print("C U:");
  lcd.print(umidade, 0);
  lcd.print("%   ");

  lcd.setCursor(0, 1);
  lcd.print("Solo:");
  lcd.print(pctSolo);
  lcd.print("% L:");
  lcd.print(pctLuz);
  lcd.print("%   ");
}
