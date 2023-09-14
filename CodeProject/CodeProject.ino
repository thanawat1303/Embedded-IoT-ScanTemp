#include <Wire.h>
// #include <LiquidCrystal.h>
#include <DFRobot_RGBLCD1602.h>

// LCD normal
// const int rs = 8 , enable = 9 , d1 = 1 , d2 = 2 , d3 = 3 , d4 = 4;
// LiquidCrystal lcd(rs , enable , d1 , d2 , d3 , d4);

// LCD I2C
DFRobot_RGBLCD1602 lcdI2C(16 , 2);

#define TemPin A0
#define LED_RED 7
#define LED_GREEN 6
#define LED_BLUE 5

#define ButtonUP 10
#define ButtonDOWN 11
#define ChangeSetting 12

const int DisplayAll = 0;
const int DisplayTemHigh = 1;
const int DisplayTemLow = 2; 

const int arrLED[3] = { LED_RED , LED_GREEN , LED_BLUE };
bool ProgramStart;

int temHIGH;
int temLOW;
int stateProgram;

void setup() {
  // lcdI2C.begin(16, 2);
  lcdI2C.init();
  lcdI2C.setBacklight(true);

  pinMode(ButtonUP, INPUT);
  pinMode(ButtonDOWN, INPUT);
  pinMode(ChangeSetting, INPUT);

  pinMode(LED_BLUE, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_RED, OUTPUT);

  ProgramStart = true;
  stateProgram = 0;
  temHIGH = 36;
  temLOW = 18;
}

void loop() {
  // ห้ามต่อ output ยาว ค่าจะเพี้ยน LM35
  if (digitalRead(ChangeSetting) == 1) {
    stateProgram = stateProgram + 1;
    if (stateProgram > 2) stateProgram = 0;

    lcdI2C.clear();
    lcdI2C.print("Change menu");
    while (digitalRead(ChangeSetting) == 1);
    lcdI2C.clear();

  } else if (digitalRead(ButtonUP) == 1 && (stateProgram == 1 || stateProgram == 2)) {
    switch (stateProgram) {
      case 1:
          temHIGH = temHIGH + 1;
        break;
      case 2:
          if(temLOW + 1 < temHIGH) temLOW = temLOW + 1;
        break;
    }
    DisplayLCD("", "UP");
    while (digitalRead(ButtonUP) == 1);
    lcdI2C.clear();
  } else if (digitalRead(ButtonDOWN) == 1 && (stateProgram == 1 || stateProgram == 2)) {
    switch (stateProgram) {
      case 1:
          if(temHIGH - 1 > temLOW) temHIGH = temHIGH - 1;
        break;
      case 2:
          temLOW = temLOW - 1;
        break;
    }
    DisplayLCD("", "DOWN");
    while (digitalRead(ButtonDOWN) == 1);
    lcdI2C.clear();
  }

  switch (stateProgram) {
    case DisplayAll:
        DisplayLCD("H : " + String(temHIGH) + " : " + "C : " + String(temLOW) , "Tem : " + String(analogRead(TemPin) * 0.48828125));
      break;
    case DisplayTemHigh:
        DisplayLCD("HOT : " + String(temHIGH), "");
      break;
    case DisplayTemLow:
        DisplayLCD("COOL : " + String(temLOW), "");
      break;
  }

  DisplayLedTemperature();
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

void DisplayLCD(String Row1 , String Row2) {
  lcdI2C.setCursor(0, 0);
  lcdI2C.print(Row1);
  lcdI2C.setCursor(0, 1);
  lcdI2C.print(Row2);
}

void SelectLED(int select) {
  for (int x; x < 3; x++) digitalWrite(arrLED[x], (arrLED[x] == select) ? HIGH : LOW);
}
