#include <Wire.h>
#include <U8g2lib.h>
U8G2_SSD1306_128X32_UNIVISION_F_SW_I2C u8g2(U8G2_R0, SCL, SDA, U8X8_PIN_NONE);

// Pins
const int stepPin = 3;
const int dirPin = 2;
const int potPin = A7;
const int dirSignalPin1 = A2;
const int dirSignalPin2 = A3;

// Parameters
const int potMin = 600;
const int potMax = 800;
const int avgSamples = 10;
const unsigned long oledUpdateInterval = 100;

// Globals
int potReadings[avgSamples];
int potIndex = 0;
long potSum = 0;
int potAverage = 0;
int prevPotAverage = 0;
bool direction = true;
volatile bool motorShouldRun = false;
unsigned long lastOledUpdate = 0;
bool upperEndReached = false;
bool lowerEndReached = false;
int upperEndPos = -1;
int lowerEndPos = -1;

void readAndUpdatePotAverage() {
  int val = analogRead(potPin);
  val = constrain(val, potMin, potMax);
  potSum -= potReadings[potIndex];
  potReadings[potIndex] = val;
  potSum += val;
  potIndex = (potIndex + 1) % avgSamples;
  potAverage = potSum / avgSamples;
}

void setupTimer2_OC2B_1kHz() {
  pinMode(stepPin, OUTPUT);
  noInterrupts();
  TCCR2A = 0;
  TCCR2B = 0;
  TCNT2 = 0;
  TCCR2A |= (1 << WGM21);   // CTC mode
  TCCR2A |= (1 << COM2B0);  // Toggle OC2B on match
  TCCR2B |= (1 << CS22);    // Prescaler = 64
  OCR2A = 124;              // 1 kHz with prescaler 64
  OCR2B = OCR2A;
  interrupts();
}

void enableStepping() {
  TCCR2B &= ~((1 << CS22) | (1 << CS21) | (1 << CS20));
  TCCR2B |= (1 << CS22);
  TCCR2A |= (1 << COM2B0);
}

void disableStepping() {
  TCCR2A &= ~(1 << COM2B0);
  TCCR2B &= ~((1 << CS22) | (1 << CS21) | (1 << CS20));
  digitalWrite(stepPin, LOW);
}

void stepperControl() {
  readAndUpdatePotAverage();
  bool signal1 = digitalRead(dirSignalPin1);
  bool signal2 = digitalRead(dirSignalPin2);
  motorShouldRun = false;
  if (signal1 != signal2) {
    direction = signal1;
    digitalWrite(dirPin, direction);
    if (direction) {  // Forward
      if (potAverage >= potMax) {
        upperEndReached = true;
        upperEndPos = potAverage;
      } else if (upperEndReached && (potAverage < (upperEndPos - 10))) {
        upperEndReached = false;
        upperEndPos = -1;
      }
      if (!upperEndReached && potAverage < potMax) {
        motorShouldRun = true;
        enableStepping();
      } else {
        motorShouldRun = false;
        disableStepping();
      }
      lowerEndReached = false;
      lowerEndPos = -1;
    } else {  // Backward
      if (potAverage <= potMin) {
        lowerEndReached = true;
        lowerEndPos = potAverage;
      } else if (lowerEndReached && (potAverage > (lowerEndPos + 10))) {
        lowerEndReached = false;
        lowerEndPos = -1;
      }
      if (!lowerEndReached && potAverage > potMin) {
        motorShouldRun = true;
        enableStepping();
      } else {
        motorShouldRun = false;
        disableStepping();
      }
      upperEndReached = false;
      upperEndPos = -1;
    }
  } else {
    motorShouldRun = false;
    disableStepping();
  }
}

void printStatus() {
  unsigned long nowMillis = millis();
  if (nowMillis - lastOledUpdate >= oledUpdateInterval) {
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_6x10_tr);
    u8g2.setCursor(0, 12);
    u8g2.print("Pot: ");
    u8g2.print(potAverage);
    u8g2.setCursor(0, 28);
    if (upperEndReached) {
      u8g2.print("END MAX");
    } else if (lowerEndReached) {
      u8g2.print("END MIN");
    }
    u8g2.sendBuffer();
    lastOledUpdate = millis();
    prevPotAverage = potAverage;
  }
}

void setup() {
  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);
  pinMode(potPin, INPUT);
  pinMode(dirSignalPin1, INPUT);
  pinMode(dirSignalPin2, INPUT);
  digitalWrite(dirPin, direction);
  for (int i = 0; i < avgSamples; i++) {
    potReadings[i] = analogRead(potPin);
    potReadings[i] = constrain(potReadings[i], potMin, potMax);
    potSum += potReadings[i];
  }
  potAverage = potSum / avgSamples;
  prevPotAverage = potAverage;
  setupTimer2_OC2B_1kHz();
  disableStepping();
  u8g2.begin();
}

void loop() {
  stepperControl();
  printStatus();
}