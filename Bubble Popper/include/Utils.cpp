#pragma once
#include "raylib.h"
#include "iostream"
#include "math.h"
#include <chrono>
#include <random>

int getRandomIntInRangeAccurate(int min, int max)
{
    std::random_device rd;   // Obtain a random seed from the hardware.
    std::mt19937 gen(rd());  // Initialize the random number generator.
    std::uniform_int_distribution<> distrib(min, max); // Define the range.

    return distrib(gen); // Generate and return the random integer.
}

int getRandomIntInRangeFast(int min, int max)
{
    static auto seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
    seed = (seed * 1103515245 + 12345) & 0x7FFFFFFF; // Linear Congruential Generator (LCG)

    // Convert the 31-bit random number to the desired range (inclusive)
    return min + static_cast<int>(seed) % (max - min + 1);
}


double map(double input, double low, double high, double outlow, double outhigh)
{

    if(input > high)
        input = high;
    if(input < low)
        input == low;
    double percentage = input/(high - low);
    double output = ((outhigh - outlow) * percentage) + outlow;
    return output;


}

Vector2 VectorFromAngle(float angle)
{
    Vector2 output;

    output.x = 1*cos(angle);
    output.y = 1*sin(angle);

    return normalize(output);
}

Vector2 randUnitVector()
{
    return normalize(Vector2{GetRandomValue(-100, 100), GetRandomValue(-100, 100)});
}
Vector2 RandUpVector()
{
    return normalize(Vector2{GetRandomValue(-1000, 1000), GetRandomValue(-1000, 0) });
}

