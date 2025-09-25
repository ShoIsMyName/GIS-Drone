#include <Servo.h>

Servo esc1, esc2, esc3, esc4;

int j1x, j1y, j2x, j2y;

void setup() {
  Serial.begin(9600);

  // กำหนดขาที่ใช้ต่อ ESC (เลือก 4 ขา Digital)
  esc1.attach(4);  // ESC1 -> D4
  esc2.attach(5);  // ESC2 -> D5
  esc3.attach(6);  // ESC3 -> D6
  esc4.attach(7);  // ESC4 -> D7

  // เริ่มต้น Arm ESC (ต้องมีแบตต่อ ESC)
  esc1.writeMicroseconds(1000);
  esc2.writeMicroseconds(1000);
  esc3.writeMicroseconds(1000);
  esc4.writeMicroseconds(1000);
  delay(3000); // รอ ESC arm ประมาณ 3 วินาที
}

void loop() {
  if (Serial.available() > 0) {
    String data = Serial.readStringUntil('\n');
    data.trim();

    int c1 = data.indexOf(',');
    int c2 = data.indexOf(',', c1 + 1);
    int c3 = data.indexOf(',', c2 + 1);

    if (c1 > 0 && c2 > 0 && c3 > 0) {
      j1x = data.substring(0, c1).toInt();
      j1y = data.substring(c1 + 1, c2).toInt();
      j2x = data.substring(c2 + 1, c3).toInt();
      j2y = data.substring(c3 + 1).toInt();
    }
  }

  int esc1_val = map(j1x, 0, 255, 1000, 2000);
  int esc2_val = map(j1y, 0, 255, 1000, 2000);
  int esc3_val = map(j2x, 0, 255, 1000, 2000);
  int esc4_val = map(j2y, 0, 255, 1000, 2000);

  esc1.writeMicroseconds(esc1_val);
  esc2.writeMicroseconds(esc2_val);
  esc3.writeMicroseconds(esc3_val);
  esc4.writeMicroseconds(esc4_val);

  delay(20); // หน่วงเล็กน้อยเพื่อความเสถียร
}
