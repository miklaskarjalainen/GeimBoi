#include "soundWaves.hpp"
#include <cmath>

namespace GeimBoi {

/*
    sound functions
*/

#define PI 3.14159
#define PI2 (PI * 2.0)

double SineWave(double freq, double dTime)
{
    return sin(freq * PI2 * dTime);
}

double SquareWave(double freq, double dTime)
{
    double dOutput = 1.0 * sin(freq * PI2 * dTime);

    if (dOutput > 0.0)
        return 0.1;
    return -0.1;
}

double HarmonicSquareWave(double freq, double dutycycle, double harmonics, double time)
{
    double a = .0, b = .0;
    double p = freq * PI2;

    for (double n = 1; n < harmonics; n++)
    {
        double c = n * freq * PI2 * time;
        a += sin(c) / n;
        b += sin(c - p * n) / n;
    }
    return (2.0/PI) * (a-b);
}


double TriangleWave(double freq, double dTime)
{
    return asin(sin(freq * PI2 * dTime)) * 2.0/PI;
}

double SawWave(double freq, double dTime)
{
    return (2.0/PI) * (freq * PI * fmod(dTime, 1.0/freq) - (PI / 2.0));
}

}
