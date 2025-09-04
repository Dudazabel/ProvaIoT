#include <Servo.h>
#include <esp32-hal-gpio.h>
#include <HardwareSerial.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128  // Largura do display
#define SCREEN_HEIGHT 64  // Altura do display
#define OLED_RESET    -1  // Reseta o display, se necessário

// Pinos de controle
const int trigPin = 14;      // Pino do Trigger do sensor ultrassônico (não usado aqui, mas definido)
const int echoPin = 27;      // Pino do Echo do sensor ultrassônico (não usado aqui, mas definido)
const int buttonPin = 23;    // Pino do botão
const int buzzerPin = 2;     // Pino do buzzer
const int ledVerde = 12;     // Pino do LED verde
const int ledVermelho = 13;  // Pino do LED vermelho
const int servoPin = 9;      // Pino do servo motor

Servo meuServo;  

bool portaoAberto = false;
unsigned long lastDebounceTime = 0;  
unsigned long debounceDelay = 50;    
int buttonState;                     
int lastButtonState = HIGH;          

// Inicializando o display OLED
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup() {
  Serial.begin(115200);

  // Configura os pinos
  pinMode(buzzerPin, OUTPUT);
  pinMode(ledVerde, OUTPUT);  // LED verde
  pinMode(ledVermelho, OUTPUT);  // LED vermelho
  pinMode(buttonPin, INPUT);
  pinMode(trigPin, OUTPUT);  // Pino do sensor ultrassônico (não usado aqui, mas definido)
  pinMode(echoPin, INPUT);   // Pino do sensor ultrassônico (não usado aqui, mas definido)

  meuServo.attach(servoPin);  
  meuServo.write(0);  // Inicializa o servo fechado (0 graus)

  // Inicializa os LEDs e o buzzer
  digitalWrite(buzzerPin, LOW);
  digitalWrite(ledVerde, LOW);  // LED verde apagado
  digitalWrite(ledVermelho, HIGH);  // LED vermelho aceso (não pode passar)

  // Inicializando o display OLED com I2C
  Wire.begin(21, 22); // Pinos I2C (SDA: 21, SCL: 22)

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {  // Endereço I2C padrão: 0x3C
    Serial.println(F("Falha ao inicializar o display OLED"));
    while (1);
  }

  display.display();  // Exibe uma tela inicial
  delay(2000);        // Espera 2 segundos

  display.clearDisplay();
  display.setTextSize(1);      
  display.setTextColor(SSD1306_WHITE);  
  display.setCursor(0,0);
  display.println(F("Display OLED"));
  display.println(F("Inicializado!"));
  display.display();
}

void loop() {
  int reading = digitalRead(buttonPin);  

  // Debounce para o botão
  if (reading != lastButtonState) {
    lastDebounceTime = millis();  
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    // Se o botão for pressionado
    if (reading == LOW && lastButtonState == HIGH && !portaoAberto) {
      Serial.println("Botão pressionado! Abrindo o portão...");
      
      meuServo.write(90);  // Abre o portão (servo 90 graus)
      digitalWrite(ledVerde, HIGH);  // Acende o LED verde (pode passar)
      digitalWrite(ledVermelho, LOW);  // Apaga o LED vermelho
      digitalWrite(buzzerPin, HIGH);    // Liga o buzzer
      
      // Display de status
      display.clearDisplay();
      display.setCursor(0, 0);
      display.println("Portão Aberto");
      display.display();
      
      portaoAberto = true;  
    }
  }

  if (portaoAberto) {
    delay(5000);  // Espera 5 segundos (simula o tempo do carro passando)

    meuServo.write(0);  // Fecha o portão (servo 0 graus)
    digitalWrite(ledVerde, LOW);   // Apaga o LED verde
    digitalWrite(ledVermelho, HIGH);    // Acende o LED vermelho (não pode passar)
    digitalWrite(buzzerPin, LOW);      // Desliga o buzzer

    // Display de status
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("Portão Fechado");
    display.display();
    
    portaoAberto = false;  
  }

  lastButtonState = reading;
}
