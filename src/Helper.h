struct Palette
{
  uint8_t len = 2;
  RgbColor colors[8] = {RgbColor(255, 0, 0), RgbColor(0, 255, 255)}; // max 8 colors

  RgbColor getColor(float pos)
  {
    pos = fmod(pos, 1);
    return RgbColor::LinearBlend(colors[int(pos * (len - 1))], colors[int(pos * (len - 1)) + 1], fmod(pos * (len - 1), 1));
  }
};

float randomFloat(float left, float right)
{
  return (float)random(left * 1000, right * 1000) / 1000.0f;
}

int mod(int k, int n) { return ((k %= n) < 0) ? k + n : k; }
