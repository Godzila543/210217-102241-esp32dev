#include "MasterLedController.h"

MasterLedController::MasterLedController() {}

MasterLedController &MasterLedController::getInstance()
{
    static MasterLedController instance;
    return instance;
}

void MasterLedController::updateStrip(float delta)
{
    DB.generator->update(delta, DB.palette);
}

void MasterLedController::calculateStrip()
{
    for (int i = 0; i < segments.size(); i++)
    {
        for (int j = segments[i].start; j < segments[i].end; j++)
        {
            segments[i].SetPixelColor(j, DB.generator->calculatePixel(j));
        }
    }
}

void MasterLedController::loop()
{
    DB.updatePalette();
    updateStrip(DB.delta);
    calculateStrip();
    for (int i = 0; i < segments.size(); i++)
    {
        segments[i].Show();
    }
}

void MasterLedController::addSegment(int pin, int start, int end, int direction)
{
    segments.push_back(LedStripSegment<NeoGrbFeature, NeoEsp32Rmt0Ws2812xMethod>(pin, start, end, direction));
}