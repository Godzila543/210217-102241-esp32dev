#pragma once

#include "Palette.h"
#include "Generator.h"

class GradientGenerator : public Generator
{
  float age = 0.0f;
  float rate = 1.0f;
  float density = 0.5f;
  Palette palette;

public:
  void setPreset(float newRate, float newDensity)
  {
    rate = newRate;
    density = newDensity;
    age = 0;
  }

  void update(float delta, Palette p) override
  {
    age += rate * delta;
    palette = p;
  }

  RgbColor calculatePixel(int index) override
  {
    return palette.getColor(density * (index / 900.0f + age));
  }
};
