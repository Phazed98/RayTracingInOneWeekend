#pragma once

#include <random>

#define MAXFLOAT FLT_MAX
#define M_PI   3.14159265358979323846264338327950288f

float drand48()
{
	static std::random_device rd;
	static std::mt19937 mt(rd());
	static std::uniform_real_distribution<double> dist(0.0, 1.0);

	return float(dist(mt));
}