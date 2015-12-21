#ifndef POINT_H
#define POINT_H

#include <cstdlib>
#include <ctime>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include <assert.h>
#include <math.h>

using namespace std;

const unsigned int POINT_MAX_DIM = 20;

class Point
{
    public:
        Point()
        {
            srand((unsigned)time(0));
        }

        void prepareCorrelatedData(double maxValue, unsigned int total, unsigned int dim);
        void prepareAntiCorrelatedData(double maxValue, unsigned int total, unsigned int dim);
        void prepareClusteredData(double maxValue, unsigned int total, unsigned int dim);
        void prepareUniData(double maxValue, unsigned int total, unsigned int dim);

    protected:
    private:

        double getRandom();

        double normalDistribution(double x, double mu, double delta);
        double ndpoint(double minValue, double maxValue, double mu, double delta, double part);
        double udpoint(double minValue, double maxValue);
};

inline double Point::getRandom()
{
    double num = rand();
    return num / RAND_MAX;
}

inline double Point::normalDistribution(double x, double mu, double delta)
{
    double coe = 1 / delta * sqrt(2 * M_PI);
    double exponent = (x - mu) * (mu - x) / (2 * delta * delta);
    return coe * exp(exponent);
}

inline double Point::ndpoint(double minValue, double maxValue, double mu, double delta, double part)
{
    double range = maxValue - minValue;
    double value = getRandom() * range;
    double fullmark = normalDistribution(0, mu, delta);
    while(true)
    {
        double x = (2 * value - range) / range * part;
        double passmark = normalDistribution(x, mu, delta);
        if(getRandom() < passmark / fullmark)
            break;
        value = getRandom() * range;
    }
    return minValue + value;
}

inline double Point::udpoint(double minValue, double maxValue)
{
    double range = maxValue - minValue;
    double value = getRandom() * range;
    return minValue + value;
}

#endif // POINT_H
