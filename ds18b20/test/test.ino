#include <OneWire.h>
#include <DallasTemperature.h>

// กำหนด one wire bus ต่อ Data เข้า pin 2 ของ Arduino
#define ONE_WIRE_BUS 9

// Setup OneWire เพื่อสื่อสารกับ Arduino
OneWire oneWire(ONE_WIRE_BUS);

// ตั้งชื่อเซ็นเซอร์เป็น DSTEMP และอ้างอิง OneWire ไปยังไลบรารี DallasTemperature
DallasTemperature DSTEMP(&oneWire);

void setup(void) {
  Serial.begin(9600);
  DSTEMP.begin();  // DSTEMP เริ่มทำงาน
}

void loop(void) {
  // ส่งคำสั่งขอข้อมูลอุณหภูมิ
  DSTEMP.requestTemperatures();

  // แสดงค่าในหน่วย Celsius
  Serial.print("Temperature: ");
  Serial.print(DSTEMP.getTempCByIndex(0));
  Serial.print("Celsius | ");

  // แสดงค่าในหน่วย Fahrenheit โดยใช้สูตรแปลง F = 1.8C + 32
  Serial.print((DSTEMP.getTempCByIndex(0) * 9.0) / 5.0 + 32.0);
  Serial.println("Farenheit");

  delay(1000);
}