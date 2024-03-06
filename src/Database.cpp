#include "Database.h"

void Database::updatePalette()
{
    float blendPercent = static_cast<float>(cyclesSincePalette) / 100.0;
    trueBrightness += (brightness - trueBrightness) / 30.0;
    blendPercent = blendPercent > 1 ? 1 : blendPercent;
    palette.blend(lastPalette, nextPalette, blendPercent);
    palette.setBrightness(trueBrightness);
    cyclesSincePalette++;
}

void Database::JSONtoPreset(char *JSONstr)
{
    DynamicJsonDocument doc(2048);
    deserializeJson(doc, JSONstr);
    if (doc["generator"] == 0)
    {
        gradientGenerator.setPreset(doc["rate"], doc["density"]);
        generator = &gradientGenerator;
    }
    else if (doc["generator"] == 1)
    {
        ParticleSettings set;
        set.particleDecay = doc["particleDecay"];
        set.timerDecay = doc["timerDecay"];
        set.intensityMethod = (IntensityMethod)(int)doc["intensityMethod"];
        set.intensityValue = doc["intensityValue"];
        set.colorMethod = (ColorMethod)(int)doc["colorMethod"];
        set.peakRange = doc["range"];
        set.curveFactor = doc["curveFactor"];
        set.referenceDecay = doc["referenceDecay"];
        set.posInitMethod = (AttrInitMethod)(int)doc["posMethod"];
        set.velInitMethod = (AttrInitMethod)(int)doc["velMethod"];
        set.posInitValue1 = doc["posValue1"];
        set.posInitValue2 = doc["posValue2"];
        set.velInitValue1 = doc["velValue1"];
        set.velInitValue2 = doc["velValue2"];
        set.calculatedAttribute = (DerivativeLevel)(int)doc["calculatedAttribute"];
        set.calcMethod = (AttrCalcMethod)(int)doc["attributeMethod"];
        set.attrValue1 = doc["aValue1"];
        set.attrValue2 = doc["aValue2"];
        set.attrValue3 = doc["aValue3"];

        particleGenerator.setPreset(set);
        generator = &particleGenerator;
    }
}
void Database::JSONtoPalette(char *JSONstr)
{
    lastPalette = palette;
    nextPalette = Palette(JSONstr);
    cyclesSincePalette = 0;
}

void Database::JSONtoBrightness(char *JSONstr)
{
    DynamicJsonDocument doc(128);
    deserializeJson(doc, JSONstr);
    brightness = doc["brightness"];
}

Database &Database::getInstance()
{
    static Database instance;
    return instance;
}

Database::Database()
{
    brightness = 1;
    trueBrightness = 1;
    delta = 1;
    cyclesSincePalette = 0;
    generator = &gradientGenerator;
}