#include <WiFi.h>
#include <WebServer.h>
#include <WebSocketsServer.h>

const char* ssid = "ESP32_Joystick";
const char* password = "12345678";

WebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(81);

// ค่าจอยสติ๊
int joyLX = 0, joyLY = 0; // Joystick Left
int joyRX = 0, joyRY = 0; // Joystick Right

const char webpage[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta charset="utf-8" />
  <title>ESP32 Drone Joystick</title>
  <style>
    body { display: flex; justify-content: space-around; align-items: center; height: 100vh; margin: 0; background: #111; color: #fff; }
    canvas { background: #222; border-radius: 50%; touch-action: none; }
  </style>
</head>
<body>
  <canvas id="joyL" width="200" height="200"></canvas>
  <canvas id="joyR" width="200" height="200"></canvas>

<script>
let ws = new WebSocket("ws://" + location.hostname + ":81/");

function joystick(id, callback) {
  let canvas = document.getElementById(id);
  let ctx = canvas.getContext("2d");
  let rect = canvas.getBoundingClientRect();
  let center = { x: canvas.width/2, y: canvas.height/2 };
  let knob = { x: center.x, y: center.y, r: 30 };
  let pressed = false;

  function draw() {
    ctx.clearRect(0,0,canvas.width,canvas.height);
    // วงกลมพื้นหลัง
    ctx.beginPath(); ctx.arc(center.x, center.y, 90, 0, Math.PI*2); ctx.strokeStyle="#555"; ctx.lineWidth=5; ctx.stroke();
    // ปุ่ม
    ctx.beginPath(); ctx.arc(knob.x, knob.y, knob.r, 0, Math.PI*2); ctx.fillStyle="#0f0"; ctx.fill();
  }

  function setPos(x,y){
    let dx = x - center.x;
    let dy = y - center.y;
    let dist = Math.sqrt(dx*dx + dy*dy);
    let max = 90;
    if (dist > max) { dx = dx/dist*max; dy = dy/dist*max; }
    knob.x = center.x + dx;
    knob.y = center.y + dy;
    let valX = Math.round((dx/max)*90); // -90 → 90
    let valY = Math.round((-dy/max)*90); // -90 → 90 (กลับด้าน Y)
    callback(valX, valY);
    draw();
  }

  canvas.addEventListener("mousedown", e=>{ pressed=true; setPos(e.offsetX,e.offsetY); });
  canvas.addEventListener("mousemove", e=>{ if(pressed) setPos(e.offsetX,e.offsetY); });
  window.addEventListener("mouseup", e=>{ pressed=false; knob.x=center.x; knob.y=center.y; callback(0,0); draw(); });

  canvas.addEventListener("touchstart", e=>{ pressed=true; let t=e.touches[0]; setPos(t.clientX-rect.left,t.clientY-rect.top); });
  canvas.addEventListener("touchmove", e=>{ let t=e.touches[0]; if(pressed) setPos(t.clientX-rect.left,t.clientY-rect.top); });
  canvas.addEventListener("touchend", e=>{ pressed=false; knob.x=center.x; knob.y=center.y; callback(0,0); draw(); });

  draw();
}

joystick("joyL",(x,y)=>{ ws.send("L,"+x+","+y); });
joystick("joyR",(x,y)=>{ ws.send("R,"+x+","+y); });
</script>
</body>
</html>
)rawliteral";

void handleRoot() {
  server.send_P(200, "text/html", webpage);
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
  if (type == WStype_TEXT) {
    String msg = (char*)payload;
    if (msg.startsWith("L,")) {
      sscanf(msg.c_str(),"L,%d,%d",&joyLX,&joyLY);
    } else if (msg.startsWith("R,")) {
      sscanf(msg.c_str(),"R,%d,%d",&joyRX,&joyRY);
    }
    Serial.printf("JoyL(%d,%d) JoyR(%d,%d)\n", joyLX,joyLY,joyRX,joyRY);
  }
}

void setup() {
  Serial.begin(115200);
  WiFi.softAP(ssid, password);
  server.on("/", handleRoot);
  server.begin();
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
  Serial.println("Joystick Web ready!");
  Serial.println(WiFi.softAPIP());
}

void loop() {
  server.handleClient();
  webSocket.loop();
}
