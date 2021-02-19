#include <Arduino.h>
#include <WiFi.h>
#include <SPIFFS.h>
#include <ESPAsyncWebServer.h>
#include <WebSocketsServer.h>
#include <ArduinoJson.h>
#include <NeoPixelBus.h>
#include "Led.h"
#include "Web.h"

void setup()
{
  Serial.begin(115200);
  webInit();
  LEDSetup();
}

void loop()
{

  // Look for and handle WebSocket data

  webLoop();
  LEDLoop();
}
