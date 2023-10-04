#include <Wire.h>
// #include <LiquidCrystal.h>

// LCD normal
// const int rs = 8 , enable = 9 , d1 = 1 , d2 = 2 , d3 = 3 , d4 = 4;
// LiquidCrystal lcd(rs , enable , d1 , d2 , d3 , d4);

// LCD I2C on Proteus
// #include <DFRobot_RGBLCD1602.h>
// DFRobot_RGBLCD1602 lcdI2C(16, 2);

// LCD I2C on Board
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcdI2C(0x27, 16, 2);

#define TemPin A0
#define LED_RED 7
#define LED_GREEN 6
#define LED_BLUE 5

#define ButtonUP 10
#define ButtonDOWN 11

const int PressPUSH = 10;
const int PressSUBT = 01;

const int DisplayAll = 0;
const int DisplayTemHigh = 1;
const int DisplayTemLow = 2;

const int arrLED[3] = { LED_RED, LED_GREEN, LED_BLUE };
bool ProgramStart;

float TemperatureGet;
int temHIGH;
int temLOW;
int stateProgram;

unsigned long currentTimeTempGet = millis();
const int IntervalPress = 2000;
const int IntervalTempGet = 1000;

String HexPress;
unsigned long TimeStart;
unsigned long TimePress;

void setup() {
  // lcdI2C.begin(16, 2);
  lcdI2C.init();
  lcdI2C.setBacklight(true);

  pinMode(ButtonUP, INPUT);
  pinMode(ButtonDOWN, INPUT);

  pinMode(LED_BLUE, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_RED, OUTPUT);

  ProgramStart = true;
  stateProgram = 0;
  temHIGH = 36;
  temLOW = 18;

  TemperatureGet = 0;
  Serial.begin(9600);
}

void loop() {
  // ห้ามต่อ output ยาว ค่าจะเพี้ยน LM35
  if (digitalRead(ButtonUP) || digitalRead(ButtonDOWN)) {
    TimePress = millis() - TimeStart;
    if (stateProgram) HexPress = String(digitalRead(ButtonUP)) + String(digitalRead(ButtonDOWN));
    if (TimePress > IntervalPress) {
      stateProgram = stateProgram + 1;
      if (stateProgram > DisplayTemLow) stateProgram = 0;
      lcdI2C.clear();
      DisplayLCDTemperature();
      while (digitalRead(ButtonUP) || digitalRead(ButtonDOWN));
    }
  } else {
    if (stateProgram && TimePress <= IntervalPress) {
      switch (HexPress.toInt()) {
        case PressPUSH:
          switch (stateProgram) {
            case DisplayTemHigh:
              temHIGH = temHIGH + 1;
              break;
            case DisplayTemLow:
              if (temLOW + 1 < temHIGH) temLOW = temLOW + 1;
              break;
          }
          break;
        case PressSUBT:
          switch (stateProgram) {
            case DisplayTemHigh:
              if (temHIGH - 1 > temLOW) temHIGH = temHIGH - 1;
              break;
            case DisplayTemLow:
              temLOW = temLOW - 1;
              break;
          }
          break;
        default: break;
      }
    }
    TimeStart = millis();
    TimePress = 0;
    HexPress = "";
  }

  if (millis() - currentTimeTempGet > IntervalTempGet) {
    TemperatureGet = analogRead(TemPin) * 0.48828125;
    currentTimeTempGet = millis();
  }

  DisplayLCDTemperature();
  DisplayLedTemperature();
}

void DisplayLCDTemperature() {
  switch (stateProgram) {
    case DisplayAll:
      DisplayLCD("H : " + String(temHIGH) + " : " + "C : " + String(temLOW), "Tem : " + String(TemperatureGet));
      break;
    case DisplayTemHigh:
      DisplayLCD("HOT : " + String(temHIGH), "");
      break;
    case DisplayTemLow:
      DisplayLCD("COOL : " + String(temLOW), "");
      break;
  }
}

void DisplayLedTemperature() {
  float temperature = analogRead(TemPin) * 0.48828125;  // ((analog / 1024) * 5000) / 10
  if (ProgramStart) {
    delay(100);
    ProgramStart = false;
  }

  if (temperature >= temHIGH) SelectLED(LED_RED);
  else if (temperature > temLOW && temperature < temHIGH) SelectLED(LED_GREEN);
  else if (temperature <= temLOW) SelectLED(LED_BLUE);
  else {
    digitalWrite(LED_RED, HIGH);
    digitalWrite(LED_GREEN, HIGH);
    digitalWrite(LED_BLUE, HIGH);
  }
}

void DisplayLCD(String Row1, String Row2) {
  lcdI2C.setCursor(0, 0);
  lcdI2C.print(Row1);
  lcdI2C.setCursor(0, 1);
  lcdI2C.print(Row2);
}

void SelectLED(int select) {
  for (int x; x < 3; x++) digitalWrite(arrLED[x], (arrLED[x] == select) ? HIGH : LOW);
}
