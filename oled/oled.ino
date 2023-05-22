#include <SPI.h>
#include <WiFi.h>
#include <WebSocketsServer.h>
#include "ESPAsyncWebServer.h"
#include "SPIFFS.h"
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "Ball.h"
#include "Pad.h"

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3D ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32

#define MAXBOUNCEANGLE 5 * PI / 12

const char* ssid     = "ESP32-DEV";
const char* password = "";

AsyncWebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(81);

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

Ball ball(SCREEN_WIDTH / 6, SCREEN_HEIGHT / 2, 2, 2, &display);
Pad pad1(3, 23, 2, 12, &display);
Pad pad2(SCREEN_WIDTH - 3, 23, 2, 12, &display);


float speedInc = 0.01;
float gameSpeed = speedInc;

bool player1Connected = false;
bool player2Connected = false;

int player1Score = 0;
int player2Score = 0;

void setup() {
  Serial.begin(115200);

  // AP init
  Serial.print("Setting AP (Access Point)…");
  WiFi.softAP(ssid, password);
  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);
  
  // Websockets init
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);

  // SPIFFS init
  if(!SPIFFS.begin(true)){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.display();
  delay(500);
  display.clearDisplay(); 
  
  // Serve static files
  server.serveStatic("/static/", SPIFFS, "/static/");
  server.on("/test", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html", "text/html");
  });
  server.begin();
}

void loop() {
  display.clearDisplay();
  
  ball.move();
  
  // Ball out of bounds
  if (ball.x < 0 || ball.x + ball.pxSize > display.width()) {
    if (ball.x < 0) {
      writeCenter("Player 2 won", display.width() / 2, display.height() / 2);
      player2Score++;
    }
    if (ball.x + ball.pxSize > display.width()) {
      writeCenter("Player 1 won", display.width() / 2, display.height() / 2);
      player1Score++;
    } 

    char score[5];
    sprintf(score, "%d:%d", player1Score, player2Score);
    writeCenter(score, display.width() / 2, display.height() / 2 + 10);
    
    display.display();
    delay(2000);
    restart();
  }

  // Ball hit on horizontal walls
  if (ball.y < 0 || ball.y > display.height() - ball.pxSize) {
    ball.y = constrain(ball.y, 1, display.height() - 1 - ball.pxSize);
    ball.velY *= -1;
  }

  // Pad hit detection
  if (ball.x <= pad1.x + pad1.width && ball.y + ball.pxSize >= pad1.y && ball.y <= pad1.y + pad1.height) {
    float intersectY = pad1.y + (pad1.height / 2) - ball.y;
    float padHalf = pad1.height / 2;
    float relativeIntersectY = intersectY / padHalf;
    float bounceAngle = relativeIntersectY * MAXBOUNCEANGLE;

    ball.velX = ball.originalVel * cos(bounceAngle) * 1.5 + gameSpeed;
    ball.velY = ball.originalVel * -sin(bounceAngle);
    
    ball.x = pad1.x + pad1.width + 1;
  }
  if (ball.x + ball.pxSize >= pad2.x && ball.y + ball.pxSize > pad2.y && ball.y < pad2.y + pad2.height) {
    float intersectY = pad2.y + (pad2.height / 2) - ball.y;
    float padHalf = pad2.height / 2;
    float relativeIntersectY = intersectY / padHalf;
    float bounceAngle = relativeIntersectY * MAXBOUNCEANGLE;

    ball.velX = -(ball.originalVel * cos(bounceAngle) * 1.5 + gameSpeed);
    ball.velY = -(ball.originalVel * sin(bounceAngle));
    
    ball.x = pad2.x - ball.pxSize;
  }

  pad1.move();
  pad2.move();
  pad1.draw();
  pad2.draw();

  ball.draw();

  gameSpeed += speedInc;
  display.display();
  webSocket.loop();
  delay(20);
}

void restart() {
  ball = Ball(SCREEN_WIDTH / 6, SCREEN_HEIGHT / 2, 2, 2, &display);
  pad1 = Pad(3, 23, 2, 12, &display);
  pad2 = Pad(SCREEN_WIDTH - 3, 23, 2, 12, &display);
  gameSpeed = speedInc;
}

void reset() {
  player1Score = 0;
  player2Score = 0;
  restart();
}

void writeCenter(char* text, int x, int y) {
  int16_t  x1, y1;
  uint16_t w, h;
  
  display.getTextBounds(text, 0, 0, &x1, &y1, &w, &h);
  display.setCursor(x - w / 2, y -  h / 2);
  display.print(text);
}

// Processing events
void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t mylength) {
    switch(type) {
        case WStype_DISCONNECTED:
            Serial.printf("[%u] Disconnected!\n", num);
            player1Connected = false;
            player2Connected = false;
            break;
        case WStype_CONNECTED:
            {
                IPAddress ip = webSocket.remoteIP(num);
                Serial.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);

                if (!player1Connected) {
                  webSocket.sendTXT(num, "Player1");  
                  player1Connected = true;
                }
                else if (!player2Connected) {
                  webSocket.sendTXT(num, "Player2");  
                  player2Connected = true;
                }
            }
            break;
        case WStype_TEXT:
            {
                //Serial.printf("[%u] get Text: %s\n", num, payload);

                char charr[mylength];
                for (int i = 0; i < mylength; i++) {
                  charr[i] = payload[i];
                }

                char player = charr[0];
                char vel[mylength - 2];

                for (int i = 0; i < mylength - 2; i++) {
                  vel[i] = charr[i + 2];
                }

                float floatVel = atof(vel);

                Serial.println(floatVel);

                if (player == '1') {
                  pad1.vel = floatVel;
                }
                else if (player == '2') {
                  pad2.vel = floatVel;
                }
    
                // send data to all connected clients
                // webSocket.broadcastTXT("message here");
            }
            break;
        case WStype_BIN:
            Serial.printf("[%u] get binary mylength: %u\n", num, mylength);
            break;
    }
}
