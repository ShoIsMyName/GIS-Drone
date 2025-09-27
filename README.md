# GIS-Drone
This drone is designed to fly and survey, collect images with GPS locations and convert them into maps that can be viewed on the web.

## Structure
<img src="https://github.com/ShoIsMyName/GIS-Drone/blob/main/GisDroneBodyFirstLayer.png?raw=true" style="width: 80%">
<img src="https://github.com/ShoIsMyName/GIS-Drone/blob/main/GisDroneAllStructure.png?raw=true" style="width: 80%">

## Arduino code
- <a href="https://github.com/ShoIsMyName/GIS-Drone/blob/main/arduino.ino" style="font-size:30px;">Arduino R3 SMD</a> <br>
- <a href="https://github.com/ShoIsMyName/GIS-Drone/blob/main/Esp32.ino" style="font-size:30px;">Esp32</a>

## Image-Stitcher 📷
>All captured images will be saved in one of the directories (can be selected by editing the settings in the source code). The images will then be imported and stitched together into a map using the cv2 library with the Stitcher function.
``` python
import cv2
import os
from colorama import Fore


################################################
#####                                      #####
#####          SETTING ALL HERE            #####
#####                                      #####
################################################

folderPath = "./ImageFolder"
resultFolderPath = "./ResultMapFolder"
verbose = True



try:
    contents = os.listdir(folderPath)
except:
    if verbose:
        print(f"[{Fore.RED}X{Fore.RESET}] No directory name \"{folderPath}\".")
    os._exit(1)

imageList = []

try:
    # append images (not filenames) to imageList
    for item in contents:
        full_path = os.path.join(folderPath, item)
        if os.path.isfile(full_path):
            img = cv2.imread(full_path)
            if img is not None:
                imageList.append(img)
except:
    if verbose:
        print(f"[{Fore.RED}X{Fore.RESET}] Can't import any file from \"{folderPath}\" directory.")
    os._exit(1)

try:
    # stitcher the picture into map
    stitcher = cv2.Stitcher_create()
    status, resultMap = stitcher.stitch(imageList)
except:
    if verbose:
        print(f"[{Fore.RED}X{Fore.RESET}] Error to stitcher.")
    os._exit(1)

# [FUNCTION] Check length file in directory
def count_files_in_directory(directory_path):
    count = 0
    for item in os.listdir(directory_path):
        item_path = os.path.join(directory_path, item)
        if os.path.isfile(item_path):
            count += 1
    return count

sequenceFileInDirectoryNumber = count_files_in_directory(resultFolderPath)

if status == cv2.Stitcher_OK:
    if verbose:
        cv2.imwrite(f"./ResultMapFolder/resultMap{sequenceFileInDirectoryNumber+1}.jpg", resultMap)
    print(f"[{Fore.GREEN}#{Fore.RESET}] Stitching complete!")
else:
    if verbose:
        print(f"[{Fore.RED}X{Fore.RESET}] Stitching failed: ", status)
```


# Remote by "Remotexy" 🎮
This is a drone controller using a bluetooth module.


> ## manual
> 1. Download the source code for board, open it in the Arduino IDE.
> 
> 2. Install the RemoteXY library for Arduino IDE.
> 
> 3. Compile the source code and upload it to the board using the Arduino IDE.
> 
> 4. Correctly connect HC-05(06) Bluetooth module to the board.
> 
> 5. Install the mobile app RemoteXY to your phone or tablet.
> 
> 6. Connect to board using mobile app.


## Controller
<img src="https://github.com/ShoIsMyName/RemotexyDroneGis/blob/main/Controller.png?raw=true" style="width: 80%">

## Source code
``` c++
#define REMOTEXY_MODE__WIFI_POINT

#include <WiFi.h>

#define REMOTEXY_WIFI_SSID "RemoteXY"
#define REMOTEXY_WIFI_PASSWORD "12345678"
#define REMOTEXY_SERVER_PORT 6377


#include <RemoteXY.h>

#pragma pack(push, 1)  
uint8_t RemoteXY_CONF[] =   // 48 bytes
  { 255,4,0,0,0,41,0,19,0,0,0,0,26,2,106,200,200,84,1,1,
  2,0,5,207,43,143,143,15,17,57,57,32,16,24,31,5,211,45,136,136,
  129,17,57,57,32,16,25,31 };
  
struct {

    // input variables
  int8_t joystick_01_x; // from -100 to 100
  int8_t joystick_01_y; // from -100 to 100
  int8_t joystick_02_x; // from -100 to 100
  int8_t joystick_02_y; // from -100 to 100

    // other variable
  uint8_t connect_flag;  // =1 if wire connected, else =0

} RemoteXY;   
#pragma pack(pop)

void setup() 
{
  RemoteXY_Init ();
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);

}

void loop() 
{
  RemoteXY_Handler ();
  
  char buffer[50];
  snprintf(buffer, sizeof(buffer), "%d,%d,%d,%d\n",
           RemoteXY.joy1_x,
           RemoteXY.joy1_y,
           RemoteXY.joy2_x,
           RemoteXY.joy2_y);

  Serial2.print(buffer); // ส่งครั้งเดียว

  RemoteXY_delay(50);

}
```


## สำรองนะ

``` c++
#include <Servo.h>
#include <Wire.h>
#include <MPU6050_light.h>

Servo esc1, esc2, esc3, esc4;
MPU6050 mpu(Wire);

int j1x, j1y, j2x, j2y;
unsigned long timer = 0;

void setup() {
  Serial.begin(9600);
  Wire.begin();

  // ====== เริ่มต้น MPU6050 ======
  byte status = mpu.begin();
  if (status != 0) {
    Serial.print("MPU6050 error: ");
    Serial.println(status);
    while (1); // ถ้าต่อไม่ถูก ให้หยุด
  }
  Serial.println("Calibrating MPU...");
  delay(2000);
  mpu.calcOffsets(); // คาลิเบรต IMU
  Serial.println("MPU Ready!");

  // ====== เริ่มต้น ESC ======
  esc1.attach(4);
  esc2.attach(5);
  esc3.attach(6);
  esc4.attach(7);

  esc1.writeMicroseconds(1000);
  esc2.writeMicroseconds(1000);
  esc3.writeMicroseconds(1000);
  esc4.writeMicroseconds(1000);
  delay(3000); // รอ ESC arm
}




void loop() {
  // ====== อ่านค่า IMU ======
  mpu.update();
  if (millis() - timer > 20) { // ทุก 20 ms
    timer = millis();
    Serial.print("Pitch: "); Serial.print(mpu.getAngleX());
    Serial.print(" | Roll: "); Serial.println(mpu.getAngleY());
  }

  // ====== อ่านค่าจาก RemoteXY ======
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

  // ====== Map joystick → ESC ======
  int esc1_val = map(j1x, 0, 255, 1000, 2000);
  int esc2_val = map(j1y, 0, 255, 1000, 2000);
  int esc3_val = map(j2x, 0, 255, 1000, 2000);
  int esc4_val = map(j2y, 0, 255, 1000, 2000);

  // ====== ตัวอย่างชดเชยมุม (ง่ายๆ) ======
  // Pitch > 0 = ก้มหน้า → เพิ่มกำลังมอเตอร์หลัง (esc3, esc4)
  float pitch = mpu.getAngleX();
  float roll  = mpu.getAngleY();
  int correctionPitch = pitch * 5; // ค่า gain เล็กๆ
  int correctionRoll  = roll  * 5;

  esc1_val -= correctionPitch + correctionRoll;
  esc2_val -= correctionPitch - correctionRoll;
  esc3_val += correctionPitch + correctionRoll;
  esc4_val += correctionPitch - correctionRoll;

  // จำกัดค่าไม่ให้เกินช่วง ESC
  esc1_val = constrain(esc1_val, 1000, 2000);
  esc2_val = constrain(esc2_val, 1000, 2000);
  esc3_val = constrain(esc3_val, 1000, 2000);
  esc4_val = constrain(esc4_val, 1000, 2000);

  // ====== ส่งค่าไป ESC ======
  esc1.writeMicroseconds(esc1_val);
  esc2.writeMicroseconds(esc2_val);
  esc3.writeMicroseconds(esc3_val);
  esc4.writeMicroseconds(esc4_val);

  delay(20); // หน่วงเล็กน้อยเพื่อความเสถียร
}

```

