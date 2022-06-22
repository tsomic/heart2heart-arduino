#include <ESP8266WiFi.h>
#include <WebSocketsClient.h>
#include <ArduinoJson.h>

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

WebSocketsClient webSocket;

int currentMode = 1;

void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);

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
}

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
  if(type == WStype_TEXT) {
    Serial.println("received message:");
    Serial.println((char *)payload);

    StaticJsonDocument<200> json;
    
    deserializeJson(json, payload);

    const int mode = json["mode"];
    const bool pressed = json["pressed"];

    currentMode = mode;

    if (pressed == 1) {
      digitalWrite(LED_BUILTIN, LOW);
      delay(500);
      digitalWrite(LED_BUILTIN, HIGH);
    } else {
      for (int i = 0; i < currentMode; i++) {
        digitalWrite(LED_BUILTIN, LOW);
        delay(200);
        digitalWrite(LED_BUILTIN, HIGH);
        delay(200);
      }
    }
  }
}

void send(int mode, bool pressed) {
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

void loop() {
  webSocket.loop();
}