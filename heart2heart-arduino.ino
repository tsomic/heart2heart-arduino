#include <ESP8266WiFi.h>
#include <WebSocketsClient.h>
#include <ArduinoJson.h>
#include <OneButton.h>

#include "ServoController.h"
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
#define BUTTON 13
#define SWITCH 14
#define NUMBER_MODES 2

int16_t beatTimer = -1;
WebSocketsClient webSocket;

OneButton button(BUTTON, false);
uint8_t currentMode = 1;

void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);

  pinMode(SWITCH, INPUT);
  
  ServoController.init();
  LEDController.init(currentMode);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address:");
  Serial.println(WiFi.localIP());

  webSocket.setExtraHeaders(headers);
  webSocket.begin(host, port);
  webSocket.onEvent(webSocketEvent);

  webSocket.setReconnectInterval(5000);
  button.attachClick(buttonPress);
  button.attachLongPressStart(buttonLongPress);
}

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
  if (type == WStype_TEXT) {
    StaticJsonDocument<200> json;

    deserializeJson(json, payload);

    const uint8_t mode = json["mode"];
    const bool pressed = json["pressed"];

    currentMode = mode;
    LEDController.changePaletteConfig(mode - 1);

    if (pressed == 1) {
      handlePress();
    }
  }
}

void send(uint8_t mode, bool pressed) {
  StaticJsonDocument<200> json;

  json["mode"] = mode;
  json["pressed"] = pressed;

  String output;
  serializeJson(json, output);

  webSocket.sendTXT(output);
}

void buttonPress() {
  LEDController.reset();
  LEDController.changePaletteConfig(2);
  LEDController.start();
  send(currentMode, true);
  hideHeart();
}

void buttonLongPress() {
  currentMode = currentMode == NUMBER_MODES ? 1 : currentMode + 1;
  bool pressed = false;
  LEDController.reset();
  if (currentMode == 1) {
    LEDController.changePaletteConfig(3);
  } else if (currentMode == 2) {
    LEDController.changePaletteConfig(4);
    pressed = true;
  }
  LEDController.start();
  send(currentMode, pressed);
  hideHeart();
}

void handlePress() {
  if (currentMode == 1) {
    activateMode1();
  } else if (currentMode == 2) {
    beatTimer = -1;
    activateMode2();
  }
}

void activateMode1() {
  showHeart();
  beatTimer = BEAT_DURATION;
}

void activateMode2() {
  showHeart();
}

void showHeart() {
  ServoController.showHeart();
  LEDController.start();
}

void hideHeart() {
  ServoController.hideHeart();
  LEDController.stop();
}

void loop() {
  if (beatTimer > 0) {
    beatTimer--;
  } else if (beatTimer == 0) {
    beatTimer--;
    hideHeart();
  }

  if (digitalRead(SWITCH) == HIGH) {
    LEDController.activateNightMode();
    ServoController.activateNightMode();
  } else {
    LEDController.disableNightMode();
    ServoController.disableNightMode();
  }

  button.tick();
  LEDController.tick();
  ServoController.tick();
  webSocket.loop();
}
