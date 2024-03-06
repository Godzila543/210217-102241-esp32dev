#pragma once

#include <NeoPixelBus.h>

/**
 * @struct Palette
 * @brief This struct represents a palette of colors.
 *
 * A palette can contain up to 8 colors. Each color can be adjusted for brightness.
 */
class Palette
{
    uint8_t len = 8;                      ///< The number of colors in the palette. Default is 8.
    RgbColor colors[8];                   ///< The array of colors. Max 8 colors.
    RgbColor brightnessAdjustedColors[8]; ///< The array of brightness-adjusted colors.
public:
    /**
     * @brief Get the color at a specific position in the palette.
     *
     * @param pos The position in the palette.
     * @return RgbColor The color at the specified position.
     */
    RgbColor getColor(float pos);

    /**
     * @brief Blend two palettes into this palette.
     *
     * @param sourcePalette The palette to blend from.
     * @param targetPalette The palette to blend to.
     * @param factor The blend factor.
     */
    void blend(Palette sourcePalette, Palette targetPalette, float factor);

    /**
     * @brief Set the brightness for all colors in the palette.
     *
     * @param brightness The brightness level.
     */
    void setBrightness(float brightness);

    Palette(){};
    Palette(char *JSONstr);
};