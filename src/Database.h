#pragma once

#include "Preferences.h"
#include "Generator.h"
#include <ArduinoJson.h>
#include "Particle.h"
#include "Gradient.h"

class Database
{
private:
    ParticleGenerator particleGenerator;
    GradientGenerator gradientGenerator;
    Palette lastPalette;
    Palette nextPalette;
    float brightness;
    int cyclesSincePalette;
    Preferences store;
    Database(); // Private constructor to prevent instantiation
    Database(const Database &) = delete;
    Database &operator=(const Database &) = delete;

public:
    static Database &getInstance(); // Method to get the singleton instance

    Generator *generator; // Database will only store the active preset and palette. It is up to the browser to send the correct information
    Palette palette;

    float trueBrightness;
    float delta;

    void updatePalette();
    void JSONtoPreset(char *JSONstr);
    void JSONtoPalette(char *JSONstr);
    void JSONtoBrightness(char *JSONstr);
};