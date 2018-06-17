#pragma once
#include <math.h>

//interpolate methods

double linearInterpolate(double a, double b, double x);

double cosineInterpolate(double a, double b, double x);

double cubicInterpolate(double v0, double v1, double v2, double v3, double x);

//1D noise functions

double noise1D(int x);

double smoothNoise1D(int x);

double interpolatedNoise1D(double x);

double perlinNoise1D(double x);

//2D noise functions

double noise2D(int x, int y);

double smoothNoise2D(int x, int y);

double interpolatedNoise2D(double x, double y);

double perlinNoise2D(double x, double y);
