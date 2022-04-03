#define strip_PixelCount 300
#define strip1_PixelPin 25
#define strip2_PixelPin 26
#define strip3_PixelPin 27

#define totalPixelCount 900

NeoPixelBus<NeoGrbFeature, NeoEsp32Rmt0Ws2812xMethod> strip1(strip_PixelCount, strip1_PixelPin);
NeoPixelBus<NeoGrbFeature, NeoEsp32Rmt1Ws2812xMethod> strip2(strip_PixelCount, strip2_PixelPin);
NeoPixelBus<NeoGrbFeature, NeoEsp32Rmt2Ws2812xMethod> strip3(strip_PixelCount, strip3_PixelPin);

NeoBuffer<NeoBufferMethod<NeoGrbFeature>> strip0(totalPixelCount, 1, NULL);

NeoTopology<RowMajorLayout> topo(totalPixelCount, 1);
uint16_t LayoutMap(int16_t x, int16_t y)
{
	return topo.MapProbe(x, y);
}

enum genType
{
	GRADIENT,
	PARTICLE
};

struct Database
{
	GradientGenerator gradientGenerator; // Database will only store the active preset and palette. It is up to the browser to send the correct information
	ParticleGenerator particleGenerator;
	Palette palette;
	Palette lastPalette;
	Palette nextPalette;
	genType gen = PARTICLE;
	float brightness = 1;
	float trueBrightness = 1;
	float delta = 1;
	int cyclesSincePalette = 0;

	void updatePalette()
	{
		float blendPercent = static_cast<float>(cyclesSincePalette) / 100.0;
		trueBrightness += (brightness - trueBrightness) / 30.0;
		blendPercent = blendPercent > 1 ? 1 : blendPercent;
		palette.blend(lastPalette, nextPalette, blendPercent);
		palette.setBrightness(trueBrightness);
		particleGenerator.setPalette(palette); // FIXME this some dumb shit
		cyclesSincePalette++;
	}
	Database()
	{
	}
};

Database DB;

void JSONtoPalette(char *JSONstr)
{
	DynamicJsonDocument doc(512);
	deserializeJson(doc, JSONstr);

	Palette tmpPalette;
	tmpPalette.len = doc["length"];
	JsonArray colors = doc["colors"].as<JsonArray>();
	for (int i = 0; i < tmpPalette.len; i++)
	{
		tmpPalette.colors[i] = RgbColor(colors[i][0], colors[i][1], colors[i][2]);
	}
	tmpPalette.setBrightness(1.0);
	for (int i = 0; i < DB.nextPalette.len; i++)
	{
		DB.nextPalette.colors[i] = tmpPalette.getColor(0.999 * ((float)i) / ((float)(DB.nextPalette.len - 1)));
	}

	for (int i = 0; i < DB.lastPalette.len; i++)
	{
		DB.lastPalette.colors[i] = DB.palette.colors[i];
	}

	DB.cyclesSincePalette = 0;
}

void JSONtoPreset(char *JSONstr)
{
	DynamicJsonDocument doc(2048);
	deserializeJson(doc, JSONstr);
	if (doc["generator"] == 0)
	{
		DB.gradientGenerator.setPreset(doc["rate"], doc["density"]);
		DB.gen = GRADIENT;
	}
	else if (doc["generator"] == 1)
	{
		ParticleSettings set;
		set.particleDecay = doc["particleDecay"];
		set.timerDecay = doc["timerDecay"];
		set.intensityMethod = (IntensityMethod)(int)doc["intensityMethod"];
		set.intensityValue = doc["intensityValue"];
		set.colorMethod = (ColorMethod)(int)doc["colorMethod"];
		set.peakRange = doc["range"];
		set.curveFactor = doc["curveFactor"];
		set.referenceDecay = doc["referenceDecay"];
		set.posInitMethod = (AttrInitMethod)(int)doc["posMethod"];
		set.velInitMethod = (AttrInitMethod)(int)doc["velMethod"];
		set.posInitValue1 = doc["posValue1"];
		set.posInitValue2 = doc["posValue2"];
		set.velInitValue1 = doc["velValue1"];
		set.velInitValue2 = doc["velValue2"];
		set.calculatedAttribute = (DerivativeLevel)(int)doc["calculatedAttribute"];
		set.calcMethod = (AttrCalcMethod)(int)doc["attributeMethod"];
		set.attrValue1 = doc["aValue1"];
		set.attrValue2 = doc["aValue2"];
		set.attrValue3 = doc["aValue3"];

		DB.particleGenerator.setPreset(set);
		DB.gen = PARTICLE;
	}
}

void JSONtoBrightness(char *JSONstr)
{
	DynamicJsonDocument doc(128);
	deserializeJson(doc, JSONstr);
	DB.brightness = doc["brightness"];
}