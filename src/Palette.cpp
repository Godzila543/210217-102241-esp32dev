#include "Palette.h"
#include <ArduinoJson.h>

RgbColor Palette::getColor(float pos)
{
    pos = fmod(pos, 1);
    return RgbColor::LinearBlend(brightnessAdjustedColors[int(pos * (len - 1))], brightnessAdjustedColors[int(pos * (len - 1)) + 1], static_cast<float>(fmod(pos * (len - 1), 1)));
}

void Palette::blend(Palette sourcePalette, Palette targetPalette, float factor)
{
    for (int i = 0; i < len; i++)
        colors[i] = RgbColor::LinearBlend(sourcePalette.colors[i], targetPalette.colors[i], factor);
}

void Palette::setBrightness(float brightness)
{
    for (int i = 0; i < len; i++)
        brightnessAdjustedColors[i] = RgbColor::LinearBlend(RgbColor(0), colors[i], brightness);
}

Palette::Palette(char *JSONstr)
{
    DynamicJsonDocument doc(512);
    deserializeJson(doc, JSONstr);

    Palette tmpPalette;
    tmpPalette.len = doc["length"];
    JsonArray jsonColors = doc["colors"].as<JsonArray>();
    for (int i = 0; i < tmpPalette.len; i++)
    {
        tmpPalette.colors[i] = RgbColor(jsonColors[i][0], jsonColors[i][1], jsonColors[i][2]);
    }
    tmpPalette.setBrightness(1.0);

    len = 8;
    for (int i = 0; i < len; i++)
    {
        colors[i] = tmpPalette.getColor(0.999 * ((float)i) / ((float)(len - 1)));
    }
}