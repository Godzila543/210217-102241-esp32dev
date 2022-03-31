#define NUM_LEDS 600

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
		for (int i = 0; i < NUM_LEDS; i++)
			strip1.SetPixelColor(i, DB.gradientGenerator.calculatePixel(DB.palette, i));
		break;
	case PARTICLE:
		for (int i = 0; i < NUM_LEDS; i++)
			strip1.SetPixelColor(i, DB.particleGenerator.calculatePixel(i));
		break;
	}
}

void LEDSetup()
{
	pinMode(strip1_PixelPin, OUTPUT);

	strip1.Begin();
	// reset them to off:
	strip1.Show();

	// set the entire buffer to black color to set off for all pixels:
	// strip0.ClearTo(black);
}

void LEDLoop()
{
	DB.updatePalette();
	updateStrip(DB.delta);
	calculateStrip();
	strip1.Show();
}
