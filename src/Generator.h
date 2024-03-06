#pragma once

#include "Palette.h"
class Generator
{
public:
  virtual void update(float delta, Palette p) = 0;
  virtual RgbColor calculatePixel(int index) = 0;
};
