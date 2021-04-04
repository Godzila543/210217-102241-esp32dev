#include "Helper.h"
#include "Generator.h"
#include "Gradient.h"
#include "Particle.h"
#include "CoreLed.h"

void updateStrip(float delta)
{
  switch (DB.gen)
  {
  case GRADIENT:
    DB.gradientGenerator.update(delta);
    break;
  case PARTICLE:
    DB.particleGenerator.update(delta);
    break;

  default:
    break;
  }
}

void calculateStrip()
{
  switch (DB.gen)
  {
  case GRADIENT:
    for (int i = 0; i < 900; i++)
      strip0.SetPixelColor(i, 0, DB.gradientGenerator.calculatePixel(DB.palette, i));
    break;
  case PARTICLE:
    for (int i = 0; i < 900; i++)
      strip0.SetPixelColor(i, 0, DB.particleGenerator.calculatePixel(i));
    break;
  }
}

void LEDSetup()
{
  pinMode(strip1_PixelPin, OUTPUT);
  pinMode(strip2_PixelPin, OUTPUT);
  pinMode(strip3_PixelPin, OUTPUT);

  strip1.Begin();
  strip2.Begin();
  strip3.Begin();
  // reset them to off:
  strip1.Show();
  strip2.Show();
  strip3.Show();

  // set the entire buffer to black color to set off for all pixels:
  //strip0.ClearTo(black);
}

void LEDLoop()
{
  DB.updatePalette();
  updateStrip(DB.delta);
  calculateStrip();
  strip0.Blt(strip1, 0, 0, 0, 0, strip_PixelCount, 1, LayoutMap);
  strip0.Blt(strip2, 0, 0, strip_PixelCount, 0, strip_PixelCount, 1, LayoutMap);
  strip0.Blt(strip3, 0, 0, strip_PixelCount * 2, 0, strip_PixelCount, 1, LayoutMap);
  // send pixels from strip0 using Blt, to strip02, starting the dump at strip02's LED x,y coordinates, always 0,0 for our simple use of Blt. From the buffer starting coordinates 18,0 (y will always be 0 for our simple use of Blt. Buffer rectangle size being dumped: 18 pixels wide, 1 pixel tall. This essentially defines how many pixels get dumped. Then using the LayoutMap that reflects the size and shape of the strip00 buffer, since we're only using layout map because its a prerequisite to use the complex Blt method where we can define the source/dest pixels specifically.
  strip1.Show();
  strip2.Show();
  strip3.Show();
}
