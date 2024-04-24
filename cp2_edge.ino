#include "DHT.h"
#include <LiquidCrystal.h>

// Definição dos pinos do LCD
const int rs = 7;
const int rw = 11;
const int en = 6;
const int d4 = 12;
const int d5 = 10;
const int d6 = 9;
const int d7 = 8;

// Inicialização do LCD com os pinos definidos acima
LiquidCrystal lcd(rs, rw, en, d4, d5, d6, d7);

// Definição dos ícones personalizados
byte iconUmdFull[] = { B00000, B00100, B01110, B11111, B11111, B11111, B01110, B00000 };
byte iconUmdMid[8] = { B00000, B00100, B01010, B10001, B11111, B11111, B01110, B00000 };
byte iconUmdLow[8] = { B00000, B00100, B01010, B10001, B10001, B10001, B01110, B00000 };

byte iconTempFull[8] = { B01110, B01110, B01110, B01110, B01110, B11111, B11111, B01110 };
byte iconTempMid[8] = { B01110, B01010, B01010, B01010, B01110, B11111, B11111, B01110 };
byte iconTempLow[8] = { B01110, B01010, B01010, B01010, B01010, B10001, B10001, B01110 };

byte iconLuzFull[8] = { B00000, B00100, B01110, B11111, B11111, B11111, B01110, B01110 };
byte iconLuzMid[8] = { B00000, B00100, B01110, B10001, B11111, B11111, B01110, B01110 };
byte iconLuzLow[8] = { B00000, B00100, B01110, B10001, B10001, B10101, B01110, B01110 };

// Definição dos pinos dos LEDs e da buzina
int ledVerde = 5;      // LED verde
int ledAmarelo = 9;    // LED amarelo
int ledVermelho = 3;  // LED vermelho
int buzina = 4;        // Buzina

int intencidadeluz;    // Variável para a intensidade da luz

#define DHTPIN 2
#define DHTTYPE DHT11   // DHT 11  (AM2302), AM2321

DHT dht(DHTPIN, DHTTYPE);      // Inicialização do sensor DHT11 no pino 2

const int NUM_LEITURAS = 2; // Número de leituras para fazer a média
int leituras_temperatura[NUM_LEITURAS];
int leituras_umidade[NUM_LEITURAS];
int leituras_intensidadeluz[NUM_LEITURAS];
int contagem_leituras = 0;

unsigned long previousMillis = 0;
const long interval = 2000; // Intervalo de tempo desejado em milissegundos

void setup() {
  Serial.println(F("DHT11 example!"));
  Serial.begin(9600);
  dht.begin();

  // Configuração dos pinos dos LEDs e da buzina como saída
  pinMode(ledAmarelo, OUTPUT);
  pinMode(ledVermelho, OUTPUT);
  pinMode(ledVerde, OUTPUT);
  pinMode(buzina, OUTPUT);
  pinMode(A0, INPUT);           // Configuração do LDR como entrada

  lcd.begin(16, 2);             // Inicialização do LCD com 16 colunas e 2 linhas
  lcd.setCursor(0, 1);          // Configuração do cursor para a segunda linha
  lcd.print("LemmaTech");  
}

void loop() {
  unsigned long currentMillis = millis();

  float media_temperatura = 0;
  float media_umidade = 0;
  float media_intensidadeluz = 0;

  int LDR = analogRead(A0);
  intencidadeluz = round(map(LDR, 0, 1023, 0, 100));

  float temperatura = dht.readTemperature();
  float umidade = dht.readHumidity();

  leituras_temperatura[contagem_leituras] = temperatura;
  leituras_umidade[contagem_leituras] = umidade;
  leituras_intensidadeluz[contagem_leituras] = intencidadeluz;
  contagem_leituras++;

  if (contagem_leituras == NUM_LEITURAS) {
    media_temperatura = calcularMedia(leituras_temperatura, NUM_LEITURAS);
    media_umidade = calcularMedia(leituras_umidade, NUM_LEITURAS);
    media_intensidadeluz = calcularMedia(leituras_intensidadeluz, NUM_LEITURAS);

    if (
      media_temperatura >= 20 || media_temperatura <= 05 ||
      media_umidade <= 45 || media_umidade >= 75 ||
      media_intensidadeluz >= 55
    ) {
      digitalWrite(ledVermelho, HIGH);
      digitalWrite(ledAmarelo, LOW);
      digitalWrite(ledVerde, LOW);
      if (currentMillis - previousMillis >= interval) {
        tone(buzina, 1000, 2500);
        previousMillis = currentMillis;
      }
    } else {
      if (
        media_temperatura >= 10 && media_temperatura <= 15 &&
        media_umidade >= 50 && media_umidade <= 70 &&
        media_intensidadeluz < 40
      ) {
        digitalWrite(ledVerde, HIGH);
        digitalWrite(ledAmarelo, LOW);
        digitalWrite(ledVermelho, LOW);
      } else {
        digitalWrite(ledVerde, LOW);
        digitalWrite(ledAmarelo, HIGH);
        digitalWrite(ledVermelho, LOW);

        if (currentMillis - previousMillis >= interval) {
          tone(buzina, 500, 1500);
          previousMillis = currentMillis;
        }
      }
    }


    lcd.clear();
    lcd.setCursor(7, 1);
    lcd.print("ERR:");
    if (media_umidade >= 50 && media_umidade <= 70) {
      lcd.createChar(1, iconUmdMid); // Cria o caractere personalizado no LCD
      lcd.setCursor(0, 0);
      lcd.write(byte(1));

      lcd.setCursor(1, 0);
      lcd.print(media_umidade);
    } else if (media_umidade > 70) {
      lcd.createChar(1, iconUmdFull); // Cria o caractere personalizado no LCD
      lcd.setCursor(0, 0);
      lcd.write(byte(1));

      lcd.setCursor(1, 0);
      lcd.print(media_umidade);

      lcd.setCursor(11, 1);
      lcd.write(byte(1));
    } else {
      lcd.createChar(1, iconUmdLow); // Cria o caractere personalizado no LCD
      lcd.setCursor(0, 0);
      lcd.write(byte(1));

      lcd.setCursor(1, 0);
      lcd.print(media_umidade);

      lcd.setCursor(11, 1);
      lcd.write(byte(1));
    }

    if (media_temperatura >= 10 && media_temperatura <= 15) {
      lcd.createChar(2, iconTempMid); // Cria o caractere personalizado no LCD
      lcd.setCursor(7, 0);
      lcd.write(byte(2));

      lcd.setCursor(8, 0);
      lcd.print(media_temperatura);
    } else if (media_temperatura > 15) {
      lcd.createChar(2, iconTempFull); // Cria o caractere personalizado no LCD
      lcd.setCursor(7, 0);
      lcd.write(byte(2));

      lcd.setCursor(8, 0);
      lcd.print(media_temperatura);

      lcd.setCursor(13, 1);
      lcd.write(byte(2));
    } else {
      lcd.createChar(2, iconTempLow); // Cria o caractere personalizado no LCD
      lcd.setCursor(7, 0);
      lcd.write(byte(2));

      lcd.setCursor(8, 0);
      lcd.print(media_temperatura);

      lcd.setCursor(13, 1);
      lcd.write(byte(2));
    }

    if (media_intensidadeluz <= 40) {
      lcd.createChar(3, iconLuzLow); // Cria o caractere personalizado no LCD
      lcd.setCursor(0, 1);
      lcd.write(byte(3));

      lcd.setCursor(1, 1);
      lcd.print(media_intensidadeluz);
    } else if (media_intensidadeluz >= 50) {
      lcd.createChar(3, iconLuzFull); // Cria o caractere personalizado no LCD
      lcd.setCursor(0, 1);
      lcd.write(byte(3));

      lcd.setCursor(1, 1);
      lcd.print(media_intensidadeluz);

      lcd.setCursor(15, 1);
      lcd.write(byte(3));
    } else {
      lcd.createChar(3, iconLuzMid); // Cria o caractere personalizado no LCD
      lcd.setCursor(0, 1);
      lcd.write(byte(3));

      lcd.setCursor(1, 1);
      lcd.print(media_intensidadeluz);

      lcd.setCursor(15, 1);
      lcd.write(byte(3));
    }

    contagem_leituras = 0;
  }

  Serial.println("Luz:");
  Serial.println(intencidadeluz);

  Serial.println("Umidade");
  Serial.println(umidade);

  Serial.println("Temperatura");
  Serial.println(temperatura);

  delay(2000);
}

// Função para calcular a média dos valores de um array
float calcularMedia(int leituras[], int num_leituras) {
  float soma = 0;
  for (int i = 0; i < num_leituras; i++) {
    soma += leituras[i];
  }
  return soma / num_leituras;
}