#pragma once
#include "FastNoiseLite.h"

class Noise {
public:
	FastNoiseLite noise;

	//creates Perlin Noise Generator
	Noise();

	//gets the noise at some coords
	float getNoise(int x, int z);

};