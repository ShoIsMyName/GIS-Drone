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


## Arduino.ino na

``` c++
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

```

