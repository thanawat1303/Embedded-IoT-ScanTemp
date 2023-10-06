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

// DS18B20
#include <OneWire.h>
#include <DallasTemperature.h>
#define WIRE_BUS_DS 9
OneWire oneWire(WIRE_BUS_DS);
DallasTemperature DSTEMP(&oneWire);

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

const int count_nextState = 1;
const int count_prevState = 1;

const int arrLED[3] = { LED_RED, LED_GREEN, LED_BLUE };

float TemperatureGet;
int temHIGH;
int temLOW;
int stateProgram;

unsigned long currentTimeTempGet = millis();
const int IntervalPress = 2000;
int IntervalTempGet = 0;

unsigned long currentTimeReset = millis();
const int IntervalReset = 10000;

String HexPress;
unsigned long TimeStart;
unsigned long TimePress;

void setup() {
  Serial.begin(9600);
  DSTEMP.begin();

  // lcdI2C.begin(16, 2);
  lcdI2C.init();
  lcdI2C.setBacklight(true);

  pinMode(ButtonUP, INPUT);
  pinMode(ButtonDOWN, INPUT);

  pinMode(LED_BLUE, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_RED, OUTPUT);

  stateProgram = 0;
  temHIGH = 36;
  temLOW = 18;

  HexPress = "";
  TemperatureGet = 0;
}

void loop() {
  // ห้ามต่อ output ยาว ค่าจะเพี้ยน LM35
  if (digitalRead(ButtonUP) || digitalRead(ButtonDOWN)) {
    unsigned long milliTime = millis();
    TimePress = milliTime - TimeStart;
    HexPress = String(digitalRead(ButtonUP)) + String(digitalRead(ButtonDOWN));

    if (TimePress > IntervalPress) {
      switch (HexPress.toInt()) {
        case PressPUSH:
          stateProgram = stateProgram + count_nextState;
          if (stateProgram > DisplayTemLow) stateProgram = DisplayAll;
          break;
        case PressSUBT:
          stateProgram = stateProgram - count_prevState;
          if (stateProgram < DisplayAll) stateProgram = DisplayTemLow;
          break;
      }

      lcdI2C.clear();
      DisplayLCDTemperature();
      while (digitalRead(ButtonUP) || digitalRead(ButtonDOWN));
    }

    // รีเซ็ตเวลาโหมดการตั้งค่าขอบเขต
    currentTimeReset = milliTime;
  } else {
    if (stateProgram && TimePress <= IntervalPress) {
      // ปรับขอบเขตอุณหภูมิ
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
      }
    }
    TimeStart = millis();
    TimePress = 0;
    HexPress = "";
  }

  if (HexPress == "") {
    unsigned long milliTime = millis();
    if (milliTime - currentTimeTempGet > IntervalTempGet) {
      TemperatureGet = GetTemp();
      currentTimeTempGet = milliTime;
      if (!IntervalTempGet) IntervalTempGet = 1000;
    } 
    
    if(milliTime - currentTimeReset > IntervalReset && stateProgram) {
      stateProgram = DisplayAll;
      currentTimeReset = milliTime;
    }
  }

  DisplayLCDTemperature();
  DisplayLedTemperature();
}

float GetTemp() {
  DSTEMP.requestTemperatures();
  return DSTEMP.getTempCByIndex(0);
}

void DisplayLCDTemperature() {
  switch (stateProgram) {
    case DisplayAll:
      DisplayLCD("H : " + String(temHIGH) + " : " + "C : " + String(temLOW), "Tem : " + String(TemperatureGet));
      break;
    case DisplayTemHigh:
      DisplayLCD("OFFSET", "  HOT : " + String(temHIGH));
      break;
    case DisplayTemLow:
      DisplayLCD("OFFSET", "  COOL : " + String(temLOW));
      break;
  }
}

void DisplayLedTemperature() {
  if (TemperatureGet >= temHIGH) SelectLED(LED_RED);
  else if (TemperatureGet > temLOW && TemperatureGet < temHIGH) SelectLED(LED_GREEN);
  else if (TemperatureGet <= temLOW) SelectLED(LED_BLUE);
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