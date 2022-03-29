#include <Arduino.h>
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
	LEDLoop();
}
