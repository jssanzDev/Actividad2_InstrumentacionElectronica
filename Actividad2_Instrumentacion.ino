// =====================================================
// ACME S.A. - Actividad 2
// Ascensor inteligente con control de temperatura
// e iluminación
// =====================================================

#include <SimpleDHT.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>

// --- Pines botones de planta ---
#define BTN1 3
#define BTN2 4
#define BTN3 5
#define BTN4 6
#define BTN5 7

// --- Servo ---
#define SERVO_PIN 9

// --- Sensores ---
#define DHT_PIN 2
#define LDR_PIN A0

// --- LEDs temperatura ---
#define LED_FRIO  10
#define LED_CALOR 11

// --- LEDs iluminación (pines digitales) ---
#define LED_L1 12
#define LED_L2 13
#define LED_L3 8

// --- LCD I2C ---
LiquidCrystal_I2C lcd(0x27, 16, 2);

// --- Servo y DHT ---
Servo miServo;
SimpleDHT22 dht22(DHT_PIN);

// --- Variables ascensor ---
int plantaActual  = 1;
int plantaDestino = 1;
int anguloPlanta[6] = {0, 0, 45, 90, 135, 180};

// --- Variables temperatura ---
float tempAnterior = 0;
float tempActual   = 0;
const float TEMP_SP      = 25.0;
const float ZONA_MUERTA  = 3.0;

// --- Temporizadores ---
unsigned long tLectura = 0;
unsigned long tLCD     = 0;

// =====================================================
void setup() {
  Serial.begin(9600);

  pinMode(BTN1, INPUT_PULLUP);
  pinMode(BTN2, INPUT_PULLUP);
  pinMode(BTN3, INPUT_PULLUP);
  pinMode(BTN4, INPUT_PULLUP);
  pinMode(BTN5, INPUT_PULLUP);

  pinMode(LED_FRIO,  OUTPUT);
  pinMode(LED_CALOR, OUTPUT);
  pinMode(LED_L1, OUTPUT);
  pinMode(LED_L2, OUTPUT);
  pinMode(LED_L3, OUTPUT);

  miServo.attach(SERVO_PIN);
  miServo.write(anguloPlanta[1]);

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Sistema ACME");
  lcd.setCursor(0, 1);
  lcd.print("Iniciando...");
  delay(2000);
  lcd.clear();

  Serial.println("Planta,TempAnt,TempAct,Accion,Lux");
}

// =====================================================
int leerBoton() {
  if (digitalRead(BTN1) == LOW) { delay(50); return 1; }
  if (digitalRead(BTN2) == LOW) { delay(50); return 2; }
  if (digitalRead(BTN3) == LOW) { delay(50); return 3; }
  if (digitalRead(BTN4) == LOW) { delay(50); return 4; }
  if (digitalRead(BTN5) == LOW) { delay(50); return 5; }
  return 0;
}

// =====================================================
void moverAscensor(int destino) {
  int angActual  = anguloPlanta[plantaActual];
  int angDestino = anguloPlanta[destino];

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Subiendo a P");
  lcd.print(destino);

  if (angActual < angDestino) {
    for (int a = angActual; a <= angDestino; a += 5) {
      miServo.write(a);
      delay(80);
    }
  } else {
    for (int a = angActual; a >= angDestino; a -= 5) {
      miServo.write(a);
      delay(80);
    }
  }

  plantaActual = destino;
  Serial.print("Ascensor en planta "); Serial.println(plantaActual);
}

// =====================================================
String controlTemperatura(float temp) {
  float limMax = TEMP_SP + ZONA_MUERTA;  // 28°C
  float limMin = TEMP_SP - ZONA_MUERTA;  // 22°C
  String accion = "OK";

  if (temp > limMax) {
    digitalWrite(LED_FRIO,  HIGH);
    digitalWrite(LED_CALOR, LOW);
    accion = "ENFRIANDO";
  } else if (temp < limMin) {
    digitalWrite(LED_FRIO,  LOW);
    digitalWrite(LED_CALOR, HIGH);
    accion = "CALENTANDO";
  } else {
    digitalWrite(LED_FRIO,  LOW);
    digitalWrite(LED_CALOR, LOW);
    accion = "OK";
  }
  return accion;
}

// =====================================================
void controlIluminacion() {
  int valorLDR = analogRead(LDR_PIN);

  if (valorLDR > 700) {
    digitalWrite(LED_L1, HIGH);
    digitalWrite(LED_L2, HIGH);
    digitalWrite(LED_L3, HIGH);
  } else if (valorLDR > 400) {
    digitalWrite(LED_L1, HIGH);
    digitalWrite(LED_L2, HIGH);
    digitalWrite(LED_L3, LOW);
  } else if (valorLDR > 200) {
    digitalWrite(LED_L1, HIGH);
    digitalWrite(LED_L2, LOW);
    digitalWrite(LED_L3, LOW);
  } else {
    digitalWrite(LED_L1, LOW);
    digitalWrite(LED_L2, LOW);
    digitalWrite(LED_L3, LOW);
  }
}

// =====================================================
void loop() {

  // 1. Leer botones y mover ascensor
  int boton = leerBoton();
  if (boton != 0 && boton != plantaActual) {
    plantaDestino = boton;
    moverAscensor(plantaDestino);
  }

  // 2. Lectura de sensores cada 2 segundos
  if (millis() - tLectura > 2000) {
    tLectura = millis();

    tempAnterior = tempActual;
    float hum = 0;
    dht22.read2(&tempActual, &hum, NULL);

    String accion = controlTemperatura(tempActual);
    controlIluminacion();
    int lux = analogRead(LDR_PIN);

    Serial.print(plantaActual); Serial.print(",");
    Serial.print(tempAnterior, 1); Serial.print(",");
    Serial.print(tempActual, 1); Serial.print(",");
    Serial.print(accion); Serial.print(",");
    Serial.println(lux);
  }

  // 3. Actualizar LCD cada 1 segundo
  if (millis() - tLCD > 1000) {
    tLCD = millis();

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("P");
    lcd.print(plantaActual);
    lcd.print(" T:");
    lcd.print(tempActual, 1);
    lcd.print("C");

    lcd.setCursor(0, 1);
    lcd.print("Ant:");
    lcd.print(tempAnterior, 1);
    lcd.print(" Act:");
    lcd.print(tempActual, 1);
  }

  delay(100);
}
