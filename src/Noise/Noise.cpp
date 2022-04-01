#include "Noise.h"

Noise::Noise()
{
	noise.SetNoiseType(FastNoiseLite::NoiseType::NoiseType_Perlin);
	noise.SetFrequency(0.07);
}

float Noise::getNoise(int x, int z)
{
	return noise.GetNoise((float)x, (float)z);
}

