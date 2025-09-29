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
    body { display: flex; justify-content: space-around; align-items: center; height: 100vh; margin: 0; background: #111; color: #fff; font-family: sans-serif; }
    .joybox { display: flex; flex-direction: column; align-items: center; }
    canvas { background: #222; border-radius: 50%; touch-action: none; }
    p { margin-top: 10px; font-size: 18px; }
  </style>
</head>
<body>
  <div class="joybox">
    <canvas id="joyL" width="200" height="200"></canvas>
    <p id="valL">X1: 0 , Y1: 0</p>
  </div>
  <div class="joybox">
    <canvas id="joyR" width="200" height="200"></canvas>
    <p id="valR">X2: 0 , Y2: 0</p>
  </div>

<script>
let ws = new WebSocket("ws://" + location.hostname + ":81/");

function joystick(id, labelId, callback) {
  let canvas = document.getElementById(id);
  let ctx = canvas.getContext("2d");
  let center = { x: canvas.width/2, y: canvas.height/2 };
  let knob = { x: center.x, y: center.y, r: 30 };
  let pressed = false;
  let label = document.getElementById(labelId);

  function draw() {
    ctx.clearRect(0,0,canvas.width,canvas.height);
    ctx.beginPath(); ctx.arc(center.x, center.y, 90, 0, Math.PI*2); ctx.strokeStyle="#555"; ctx.lineWidth=5; ctx.stroke();
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
    let valX = Math.round((dx/max)*90);
    let valY = Math.round((-dy/max)*90);
    label.textContent = (id==="joyL" ? "X1: " : "X2: ") + valX + " , " + (id==="joyL" ? "Y1: " : "Y2: ") + valY;
    callback(valX, valY);
    draw();
  }

  function getPos(evt) {
    let rect = canvas.getBoundingClientRect();
    let x, y;
    if (evt.touches) {
      x = evt.touches[0].clientX - rect.left;
      y = evt.touches[0].clientY - rect.top;
    } else {
      x = evt.clientX - rect.left;
      y = evt.clientY - rect.top;
    }
    return {x,y};
  }

  canvas.addEventListener("mousedown", e=>{ pressed = true; let p=getPos(e); setPos(p.x,p.y); });
  canvas.addEventListener("mousemove", e=>{ if(pressed){ let p=getPos(e); setPos(p.x,p.y); } });
  window.addEventListener("mouseup", e=>{ pressed=false; knob.x=center.x; knob.y=center.y; callback(0,0); label.textContent=(id==="joyL"?"X1: 0 , Y1: 0":"X2: 0 , Y2: 0"); draw(); });

  canvas.addEventListener("touchstart", e=>{ pressed=true; let p=getPos(e); setPos(p.x,p.y); e.preventDefault(); });
  canvas.addEventListener("touchmove", e=>{ if(pressed){ let p=getPos(e); setPos(p.x,p.y); } e.preventDefault(); });
  canvas.addEventListener("touchend", e=>{ pressed=false; knob.x=center.x; knob.y=center.y; callback(0,0); label.textContent=(id==="joyL"?"X1: 0 , Y1: 0":"X2: 0 , Y2: 0"); draw(); e.preventDefault(); });

  draw();
}

joystick("joyL","valL",(x,y)=>{ ws.send("L,"+x+","+y); });
joystick("joyR","valR",(x,y)=>{ ws.send("R,"+x+","+y); });
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
  Serial1.begin(9600, SERIAL_8N1, -1, 17);
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

  Serial1.printf("%d,%d,%d,%d\n", joyLX, joyLY, joyRX, joyRY);
}
