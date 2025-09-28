#include <Servo.h>

Servo motorFL; // Front Left
Servo motorFR; // Front Right
Servo motorBL; // Back Left
Servo motorBR; // Back Right

String inputString = "";
boolean stringComplete = false;

void setup() {
  Serial.begin(9600);

  motorFL.attach(3); // Brown
  motorFR.attach(5); // Black
  motorBL.attach(6); // White
  motorBR.attach(9); // Gray

  inputString.reserve(50);
}

void loop() {
  if (stringComplete) {
    int j1x, j1y, j2x, j2y;
    sscanf(inputString.c_str(), "%d,%d,%d,%d", &j1x, &j1y, &j2x, &j2y);

    // map ค่าจาก joystick -100..100 → 1000..2000 (PWM ESC)
    int throttle = map(j1y, -100, 100, 1000, 2000); // ความสูง
    int yaw      = map(j1x, -100, 100, -200, 200);  // หันซ้าย-ขวา
    int pitch    = map(j2y, -100, 100, -200, 200);  // เดินหน้า-หลัง
    int roll     = map(j2x, -100, 100, -200, 200);  // เอียงซ้าย-ขวา

    // คำนวณกำลังแต่ละมอเตอร์
    int mFL = throttle + pitch - roll + yaw;
    int mFR = throttle + pitch + roll - yaw;
    int mBL = throttle - pitch - roll - yaw;
    int mBR = throttle - pitch + roll + yaw;

    // จำกัดค่าไม่ให้เกินช่วง 1000–2000
    mFL = constrain(mFL, 1000, 2000);
    mFR = constrain(mFR, 1000, 2000);
    mBL = constrain(mBL, 1000, 2000);
    mBR = constrain(mBR, 1000, 2000);

    // ส่งค่าให้ ESC
    motorFL.writeMicroseconds(mFL);
    motorFR.writeMicroseconds(mFR);
    motorBL.writeMicroseconds(mBL);
    motorBR.writeMicroseconds(mBR);

    Serial.print("FL: "); Serial.print(mFL);
    Serial.print(" FR: "); Serial.print(mFR);
    Serial.print(" BL: "); Serial.print(mBL);
    Serial.print(" BR: "); Serial.println(mBR);

    inputString = "";
    stringComplete = false;
  }
}

void serialEvent() {
  while (Serial.available()) {
    char inChar = (char)Serial.read();
    if (inChar == '\n') {
      stringComplete = true;
    } else {
      inputString += inChar;
    }
  }
}
