struct Palette
{
  uint8_t len = 2;
  RgbColor colors[8] = {RgbColor(0, 0, 0), RgbColor(255, 255, 255)}; // max 8 colors
};

RgbColor getColorFromPalette(Palette palette, float pos)
{
  pos = fmod(pos, 1);
  return RgbColor::LinearBlend(palette.colors[int(pos * (palette.len - 1))], palette.colors[int(pos * (palette.len - 1)) + 1], fmod(pos * (palette.len - 1), 1));
}

float randomFloat(float left, float right)
{
  return (float)random(left * 1000, right * 1000) / 1000.0f;
}

int mod(int k, int n) { return ((k %= n) < 0) ? k + n : k; }
