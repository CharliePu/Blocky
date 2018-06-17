#include "Noise.h"

double linearInterpolate(double a, double b, double x)
{
	return a*(1 - x) + b*x;
}

double cosineInterpolate(double a, double b, double x)
{
	double ft = x*3.1415927;
	double f = (1 - cos(ft))*0.5;
	return a*(1 - f) + b*f;
}

double cubicInterpolate(double v0, double v1, double v2, double v3, double x)
{
	double P = (v3 - v2) - (v0 - v1);
	double Q = (v0 - v1) - P;
	double R = v2 - v0;
	double S = v1;
	return P*x*x*x + Q*x*x + R*x + S;
}


int primeNumber[]
{
	13,
	17,
	23,
	29,
	31,
	37,
	41,
	43,
	47,
	53
};

int randomID;

double noise1D(int x)
{
	x = (x << 13) ^ x;
	return (1.0 - ((x * (x * x * primeNumber[randomID] + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0);
}

double smoothNoise1D(int x)
{
	return noise1D(x) / 2 + noise1D(x - 1) / 4 + noise1D(x + 1) / 4;
}

double interpolatedNoise1D(double x)
{
	int a = floor(x);
	int b = a + 1;
	return cosineInterpolate(smoothNoise1D(a), smoothNoise1D(b), x - a);
}

double perlinNoise1D(double x)
{
	double total = 0;
	double p = 0.5;
	int n = 10;
	for (int i = 0; i < n; i++)
	{
		randomID = i;
		total += interpolatedNoise1D(x*pow(2, i)) * pow(p, i);
	}
	return total;
}

double noise2D(int x, int y)
{
	int n = x + y * 57;
	n = (n << 13) ^ n;
	return  (1.0 - ((n * (n * n * primeNumber[randomID] + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0);
}

double smoothNoise2D(int x, int y)
{
	double corners = (noise2D(x - 1, y - 1) + noise2D(x + 1, y + 1) + noise2D(x + 1, y - 1) + noise2D(x - 1, y + 1)) / 16;
	double sides = (noise2D(x - 1, y) + noise2D(x + 1, y) + noise2D(x, y - 1) + noise2D(x, y + 1)) / 8;
	double center = noise2D(x, y) / 2;
	return corners + sides + center;
}

double interpolatedNoise2D(double x, double y)
{
	int intX = floor(x);
	double deltaX = x - intX;

	int intY = floor(y);
	double deltaY = y - intY;

	double v1 = smoothNoise2D(intX, intY);
	double v2 = smoothNoise2D(intX + 1, intY);
	double v3 = smoothNoise2D(intX, intY + 1);
	double v4 = smoothNoise2D(intX + 1, intY + 1);

	double i1 = cosineInterpolate(v1, v2, deltaX);
	double i2 = cosineInterpolate(v3, v4, deltaX);

	return cosineInterpolate(i1, i2, deltaY);
}

double perlinNoise2D(double x, double y)
{
	double total = 0;
	double p = 0.5;
	double n = 10;
	for (int i = 0; i < n; i++)
	{
		randomID = i;
		total += interpolatedNoise2D(x*pow(2, i), y*pow(2, i)) * pow(p, i);
	}
	return total;
}