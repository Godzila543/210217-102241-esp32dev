struct Palette
{
	uint8_t len = 2;
	RgbColor colors[8] = {RgbColor(255, 0, 0), RgbColor(0, 255, 255)}; // max 8 colors
	RgbColor brightnessAdjustedColors[8] = {RgbColor(255, 0, 0), RgbColor(0, 255, 255)};

	RgbColor getColor(float pos)
	{
		pos = fmod(pos, 1);
		return RgbColor::LinearBlend(brightnessAdjustedColors[int(pos * (len - 1))], brightnessAdjustedColors[int(pos * (len - 1)) + 1], fmod(pos * (len - 1), 1));
	}
	void blend(Palette blendFrom, Palette blendTo, float c)
	{
		for (int i = 0; i < len; i++)
			colors[i] = RgbColor::LinearBlend(blendFrom.colors[i], blendTo.colors[i], c);
	}
	void setBrightness(float bright)
	{
		for (int i = 0; i < len; i++)
			brightnessAdjustedColors[i] = RgbColor::LinearBlend(RgbColor(0), colors[i], bright);
	}
};

float randomFloat(float left, float right)
{
	return (float)random(left * 1000, right * 1000) / 1000.0f;
}

int mod(int k, int n) { return ((k %= n) < 0) ? k + n : k; }
