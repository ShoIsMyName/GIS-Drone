#define REMOTEXY_MODE__WIFI_POINT

#include <WiFi.h>
#define REMOTEXY_WIFI_SSID "RemoteXY"
#define REMOTEXY_WIFI_PASSWORD "12345678"
#define REMOTEXY_SERVER_PORT 6377

#include <RemoteXY.h>

#pragma pack(push, 1)
uint8_t RemoteXY_CONF[] =   // 168 bytes
  { 255,4,0,20,0,161,0,19,0,0,0,0,26,2,106,200,200,84,1,1,
  10,0,5,206,31,143,143,18,26,49,49,0,16,26,31,5,221,67,117,117,
  135,26,49,49,0,27,26,31,67,11,21,21,24,7,11,25,9,78,2,26,
  2,67,5,31,13,21,39,11,25,9,78,2,26,2,67,6,36,13,21,130,
  12,25,9,78,2,26,2,67,7,41,13,21,156,12,25,9,78,2,26,2,
  74,52,31,21,24,13,4,25,6,13,2,30,24,192,88,49,0,74,8,14,
  13,14,45,4,25,6,13,2,30,24,192,89,49,0,74,8,14,13,14,134,
  4,25,6,13,2,30,24,192,88,50,0,74,25,14,13,14,166,4,25,6,
  13,2,30,24,192,89,50,0 };

struct {
  int8_t joystick_01_x; // -100 to 100
  int8_t joystick_01_y; // -100 to 100
  int8_t joystick_02_x; // -100 to 100
  int8_t joystick_02_y; // -100 to 100

  // output debug variables
  float x1_para;
  float y1_para;
  float x2_para;
  float y2_para;

  uint8_t X1; 
  uint8_t strings_01; 
  uint8_t strings_02; 
  uint8_t strings_03; 

  uint8_t connect_flag;
} RemoteXY;
#pragma pack(pop)

#define RXD2 16
#define TXD2 17

int lastJ1x = 0;
int lastJ1y = 0;

void setup() {
  RemoteXY_Init();
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);
  Serial.begin(115200); // Debug Monitor
}

void loop() {
  RemoteXY_Handler();

  int newJ1x = RemoteXY.joystick_01_x; // yaw
  int newJ1y = RemoteXY.joystick_01_y; // throttle
  int newJ2x = RemoteXY.joystick_02_x; // roll
  int newJ2y = RemoteXY.joystick_02_y; // pitch

  // Left Joystick (hold last value if not moving)
  int sendJ1x = lastJ1x;
  int sendJ1y = lastJ1y;

  if (newJ1x != 0) {
    sendJ1x = newJ1x;
    lastJ1x = newJ1x;
  }
  if (newJ1y != 0) {
    sendJ1y = newJ1y;
    lastJ1y = newJ1y;
  }

  // Right Joystick (always send)
  int sendJ2x = newJ2x;
  int sendJ2y = newJ2y;

  // --- Update RemoteXY Debug values ---
  RemoteXY.x1_para = sendJ1x / 100.0f; 
  RemoteXY.y1_para = sendJ1y / 100.0f;
  RemoteXY.x2_para = sendJ2x / 100.0f;
  RemoteXY.y2_para = sendJ2y / 100.0f;

  // --- Send to Arduino R3 ---
  char buffer[80];
  snprintf(buffer, sizeof(buffer), "%d,%d,%d,%d\n",
           sendJ1x, sendJ1y, sendJ2x, sendJ2y);
  Serial2.print(buffer);

  // Debug Monitor (PC Serial)
  Serial.print("TX: ");
  Serial.print(buffer);
}
