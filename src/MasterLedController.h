#pragma once

#include "Database.h"
#include "LedStripSegment.h"
#include <vector>

class MasterLedController
{
private:
    std::vector<LedStripSegment<NeoGrbFeature, NeoEsp32Rmt0Ws2812xMethod>> segments;
    Database &DB = Database::getInstance();
    MasterLedController();
    MasterLedController(const MasterLedController &) = delete;
    MasterLedController &operator=(const MasterLedController &) = delete;

public:
    static MasterLedController &getInstance();
    void updateStrip(float delta);
    void calculateStrip();
    /**
     * @brief Add a new segment to the LED strip.
     *
     * This method creates a new LedStripSegment object and adds it to the segments vector.
     *
     * @param pin The pin number for the new segment.
     * @param start The start position for the new segment.
     * @param end The end position for the new segment.
     * @param direction The direction of the new segment.
     */
    void addSegment(int numLeds, int start, int end, int pin);
    void loop();
};