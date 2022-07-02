#include <ESP8266WiFi.h>
#include <WebSocketsClient.h>
#include <ArduinoJson.h>
#include <OneButton.h>
#include <Servo.h>

#include "LEDController.h"

// remove this line if you don't have your credentials stored in a custom 'credentials' library
#include <credentials.h>

#ifndef WIFI_SSID
#define WIFI_SSID "your-ssid"
#define WIFI_PASSWORD "your-password"
#define HEADERS "your-headers"
#define HOST "your-host"
#define PORT 80
#endif

const char* ssid = WIFI_SSID;
const char* password = WIFI_PASSWORD;
const char* headers = HEADERS;
const char* host = HOST;
const uint16_t port = PORT;

#define BEAT_DURATION 1000
#define SERVO_MAX 180
#define PIN_SERVO 5
#define PIN_BUTTON 13
#define NUMBER_MODES 2

int16_t beatTimer = -1;
WebSocketsClient webSocket;
Servo Servo_SG90;
OneButton button(PIN_BUTTON, false);
uint8_t currentMode = 1;

void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  Servo_SG90.attach(PIN_SERVO);
  Servo_SG90.write(0);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  LEDController.init(1);

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address:");
  Serial.println(WiFi.localIP());

  webSocket.setExtraHeaders(headers);
  webSocket.begin(host, port);
  webSocket.onEvent(webSocketEvent);

  webSocket.setReconnectInterval(5000);
  button.attachClick(click);
  button.attachLongPressStart(longPress);
}

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
  if (type == WStype_TEXT) {
    Serial.println("received message:");
    Serial.println((char *)payload);

    StaticJsonDocument<200> json;

    deserializeJson(json, payload);

    const uint8_t mode = json["mode"];
    const bool pressed = json["pressed"];

    currentMode = mode;
    LEDController.changePaletteConfig(mode - 1);

    if (pressed == 1) {
      handlePress();
    } else {
      for (uint8_t i = 0; i < currentMode; i++) {
        digitalWrite(LED_BUILTIN, LOW);
        delay(200);
        digitalWrite(LED_BUILTIN, HIGH);
        delay(200);
      }
    }
  }
}

void send(uint8_t mode, bool pressed) {
  Serial.println("received message:");

  StaticJsonDocument<200> json;

  json["mode"] = mode;
  json["pressed"] = pressed;

  String output;
  serializeJson(json, output);

  webSocket.sendTXT(output);

  Serial.println("sent message:");
  Serial.println(output);
}

void click() {
  LEDController.reset();
  LEDController.changePaletteConfig(2);
  LEDController.start();
  send(currentMode, true);
  if (currentMode == 2) {
    hideHeart();
  }
}

void longPress() {
  int newMode = currentMode == NUMBER_MODES ? 1 : currentMode + 1;
  currentMode = newMode;
  send(currentMode, false);
  hideHeart();
}

void handlePress() {
  if (currentMode == 1) {
    mode1Press();
  } else if (currentMode == 2) {
    beatTimer = -1;
    mode2Press();
  }
}

void mode1Press() {
  showHeart();
  beatTimer = BEAT_DURATION;
}

void mode2Press() {
  showHeart();
}

void showHeart() {
  digitalWrite(LED_BUILTIN, LOW);
  Servo_SG90.write(SERVO_MAX);
  LEDController.start();
}

void hideHeart() {
  digitalWrite(LED_BUILTIN, HIGH);
  Servo_SG90.write(0);
  LEDController.stop();
}

void loop() {
  if (beatTimer > 0) {
    beatTimer--;
  } else if (beatTimer == 0) {
    beatTimer--;
    hideHeart();
  }

  button.tick();
  LEDController.tick();
  webSocket.loop();
}
