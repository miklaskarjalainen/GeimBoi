#pragma once

namespace GeimBoi {

    /*
        sound functions
    */
    double SineWave(double freq, double dTime);
    double SquareWave(double freq, double dTime);
    double HarmonicSquareWave(double freq, double dutycycle, double harmonics, double dTime);
    double TriangleWave(double freq, double dTime);
    double SawWave(double freq, double dTime);

}
