#include <Arduino.h>
#include "MasterLedController.h"
#include "Web.h"

MasterLedController &master = MasterLedController::getInstance();

void setup()
{
	Serial.begin(115200);
	webInit();

	master.addSegment(25, 0, 600, 1);
}

void loop()
{
	master.loop();
}
