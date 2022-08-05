#include "Noise.h"

Noise::Noise()
{
	noise.SetNoiseType(FastNoiseLite::NoiseType::NoiseType_Perlin);
	noise.SetFrequency(0.005);
	noise.SetFractalType(FastNoiseLite::FractalType::FractalType_PingPong);
	noise.SetFractalOctaves(5);
	noise.SetFractalGain(0.4);
}

float Noise::getNoise(int x, int z)
{
	return noise.GetNoise((float)x, (float)z);
}

