#include <ESP32Servo.h> 
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Definição dos pinos
const int botaoPin = 23;   // Pino do botão
const int trigPin = 27;    // Pino TRIG do sensor ultrassônico
const int echoPin = 19;    // Pino ECHO do sensor ultrassônico
const int servoPin = 25;   // Pino do servo
const int ledPin1 = 15;    // LED amarelo
const int ledPin2 = 2;     // LED verde
const int buzzerPin = 26;  // Pino do buzzer

// Configuração do display OLED
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

Servo meuServo;

long duracao;
int distancia;

// Variáveis do botão
int estadoBotao = 0;
int ultimoEstadoBotao = 0;
unsigned long ultimoTempoBotao = 0;
unsigned long debounceDelay = 50;  // Delay de debounce para o botão

void setup() {
  Serial.begin(115200);
  
  pinMode(botaoPin, INPUT_PULLUP);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(ledPin1, OUTPUT);
  pinMode(ledPin2, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  
  meuServo.attach(servoPin);
  meuServo.write(0); // Inicialmente fechado
  
  digitalWrite(ledPin1, LOW);
  digitalWrite(ledPin2, HIGH);
  digitalWrite(buzzerPin, LOW);

  // Inicialização do I2C no ESP32 (SDA = 21, SCL = 22)
  Wire.begin(21, 22);

  // Inicialização do display OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("Falha ao iniciar display OLED"));
    for (;;);
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Sistema Iniciado");
  display.display();
  delay(2000);
  display.clearDisplay();
}

void loop() {
  estadoBotao = digitalRead(botaoPin);

  if (estadoBotao == LOW && ultimoEstadoBotao == HIGH && (millis() - ultimoTempoBotao) > debounceDelay) {
    Serial.println("Botao pressionado!");
    acionarServo();
    acionarLEDs(true);
    tocarBuzzer();
    mostrarDisplay("Portao aberto");
    ultimoTempoBotao = millis();
  }
  ultimoEstadoBotao = estadoBotao;

  distancia = medirDistancia();
  Serial.print("Distancia: "); Serial.println(distancia);

  if (distancia > 0 && distancia < 15) {
    Serial.println("Objeto detectado perto!");
    acionarServo();
    acionarLEDs(true);
    tocarBuzzer();
    mostrarDisplay("Objeto detectado");
    delay(500);
  }

  if (estadoBotao == HIGH && distancia > 15) {
    acionarLEDs(false);
    digitalWrite(buzzerPin, LOW);
    mostrarDisplay("Portao fechado");
  }

  delay(100);
}

int medirDistancia() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  duracao = pulseIn(echoPin, HIGH);
  distancia = duracao * 0.034 / 2;
  return distancia;
}

void acionarServo() {
  meuServo.write(90);
  delay(1000);
  meuServo.write(0);
}

void acionarLEDs(bool led1Estado) {
  if (led1Estado) {
    digitalWrite(ledPin1, HIGH);
    digitalWrite(ledPin2, LOW);
  } else {
    digitalWrite(ledPin1, LOW);
    digitalWrite(ledPin2, HIGH);
  }
}

void tocarBuzzer() {
  digitalWrite(buzzerPin, HIGH);
  delay(1000);
  digitalWrite(buzzerPin, LOW);
}

void mostrarDisplay(const char* mensagem) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println(mensagem);
  display.display();
}
