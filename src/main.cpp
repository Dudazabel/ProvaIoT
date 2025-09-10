#include <Servo.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128  
#define SCREEN_HEIGHT 64  
#define OLED_RESET    -1  

// Pinos de controle
const int trigPin = 5;        // Sensor ultrassônico TRIG
const int echoPin = 18;       // Sensor ultrassônico ECHO
const int buttonPin = 23;     // Botão
const int buzzerPin = 2;      // Buzzer
const int ledVerde = 26;      // LED verde (não pode usar GPIO34)
const int ledVermelho = 15;   // LED vermelho
const int servoPin = 25;      // Servo motor

Servo meuServo;  

bool portaoAberto = false;
unsigned long lastDebounceTime = 0;  
unsigned long debounceDelay = 50;    
unsigned long portaoAbertoTime = 0;  
int buttonState;                     
int lastButtonState = HIGH;          

// Inicializando o display OLED
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup() {
  Serial.begin(115200);

  // Configura os pinos
  pinMode(buzzerPin, OUTPUT);
  pinMode(ledVerde, OUTPUT);
  pinMode(ledVermelho, OUTPUT);
  pinMode(buttonPin, INPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  // Inicializa o servo
  meuServo.attach(servoPin);  
  meuServo.write(0);  // Portão fechado

  // Inicializa LEDs e buzzer
  digitalWrite(buzzerPin, LOW);
  digitalWrite(ledVerde, LOW);
  digitalWrite(ledVermelho, HIGH);

  // Inicializando o display OLED
  Wire.begin(21, 22); // SDA: 21, SCL: 22
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {  
    Serial.println(F("Falha ao inicializar o display OLED"));
    while (1);
  }

  display.clearDisplay();
  display.setTextSize(1);      
  display.setTextColor(SSD1306_WHITE);  
  display.setCursor(0,0);
  display.println(F("Display OLED Inicializado!"));
  display.display();
  delay(2000);
  display.clearDisplay();
}

void loop() {
  int reading = digitalRead(buttonPin);  

  // Debounce
  if (reading != lastButtonState) {
    lastDebounceTime = millis();  
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading == LOW && lastButtonState == HIGH && !portaoAberto) {
      Serial.println("Botão pressionado! Abrindo o portão...");
      
      meuServo.write(90);  // Abre o portão
      digitalWrite(ledVerde, HIGH);
      digitalWrite(ledVermelho, LOW);
      digitalWrite(buzzerPin, HIGH);
      
      display.clearDisplay();
      display.setCursor(0, 0);
      display.println("Portão Aberto");
      display.display();
      
      portaoAberto = true;  
      portaoAbertoTime = millis();
    }
  }

  lastButtonState = reading;

  // Fecha portão após 5 segundos sem bloquear o loop
  if (portaoAberto && (millis() - portaoAbertoTime >= 5000)) {
    meuServo.write(0);  // Fecha portão
    digitalWrite(ledVerde, LOW);
    digitalWrite(ledVermelho, HIGH);
    digitalWrite(buzzerPin, LOW);
    
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("Portão Fechado");
    display.display();

    portaoAberto = false;  
  }
}
