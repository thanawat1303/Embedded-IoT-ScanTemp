#include <Wire.h>
#include <LiquidCrystal.h>
#include <DFRobot_RGBLCD1602.h>

// LCD normal
const int rs = 8 , enable = 9 , d1 = 1 , d2 = 2 , d3 = 3 , d4 = 4;
LiquidCrystal lcd(rs , enable , d1 , d2 , d3 , d4);

// LCD I2C
// DFRobot_RGBLCD1602 lcdI2C(16 , 2);

#define TemPin A0
#define LED_RED 7
#define LED_GREEN 6
#define LED_BLUE 5

#define ButtonUP 10
#define ButtonDOWN 11
#define ChangeSetting 12

bool state_start;

int temHIGH;
int temLOW;
int state_change;

void setup() {
  lcd.begin(16, 2);

  pinMode(ButtonUP, INPUT);
  pinMode(ButtonDOWN, INPUT);
  pinMode(ChangeSetting, INPUT);

  pinMode(LED_BLUE, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_RED, OUTPUT);

  state_start = true;
  state_change = 0;
  temHIGH = 35;
  temLOW = 18;
}

void loop() {
  // ห้ามต่อ output ยาว ค่าจะเพี้ยน LM35
  if (digitalRead(ChangeSetting) == 1) {
    state_change = state_change + 1;
    if (state_change > 2) state_change = 0;

    lcd.clear();
    lcd.print("Change menu");
    while (digitalRead(ChangeSetting) == 1);
    lcd.clear();

  } else if (digitalRead(ButtonUP) == 1 && (state_change == 1 || state_change == 2)) {
    switch (state_change) {
      case 1:
        temHIGH = temHIGH + 1;
        break;
      case 2:
        if(temLOW + 1 < temHIGH) temLOW = temLOW + 1;
        break;
    }
    DisplayLCD("", "UP");
    while (digitalRead(ButtonUP) == 1);
    lcd.clear();
  } else if (digitalRead(ButtonDOWN) == 1 && (state_change == 1 || state_change == 2)) {
    switch (state_change) {
      case 1:
        if(temHIGH - 1 > temLOW) temHIGH = temHIGH - 1;
        break;
      case 2:
        temLOW = temLOW - 1;
        break;
    }
    DisplayLCD("", "DOWN");
    while (digitalRead(ButtonDOWN) == 1);
    lcd.clear();
  }

  switch (state_change) {
    case 0:
      DisplayLCD("H : " + String(temHIGH) + " : " + "C : " + String(temLOW) , "Tem : " + String(analogRead(TemPin) * 0.48828125));
      break;
    case 1:
      DisplayLCD("HOT : " + String(temHIGH), "");
      break;
    case 2:
      DisplayLCD("COOL : " + String(temLOW), "");
      break;
  }

  DisplayLED_temperature();
}



void DisplayLED_temperature() {
  float temperature = analogRead(TemPin) * 0.48828125;  // ((analog / 1024) * 5000) / 10
  if (state_start) {
    delay(100);
    state_start = false;
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
  lcd.setCursor(0, 0);
  lcd.print(Row1);
  lcd.setCursor(0, 1);
  lcd.print(Row2);
}

void SelectLED(int select) {
  const int arrLED[3] = { LED_RED, LED_GREEN, LED_BLUE };
  for (int x; x < 3; x++) {
    if (arrLED[x] == select) digitalWrite(arrLED[x], HIGH);
    else digitalWrite(arrLED[x], LOW);
  }
}
