class GradientGenerator : Generator
{
  float age = 0.0f;
  float rate = 1.0f;
  float density = 0.5f;

public:
  void setPreset(float newRate, float newDensity)
  {
    rate = newRate;
    density = newDensity;
    age = 0;
  }

  void update(float delta)
  {
    age += rate * delta;
  }

  RgbColor calculatePixel(Palette palette, int index)
  {
    return palette.getColor(density * (index / 900.0f + age));
  }
};
