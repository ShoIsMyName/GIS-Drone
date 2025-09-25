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
