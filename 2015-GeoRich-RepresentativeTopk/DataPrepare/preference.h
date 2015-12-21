#ifndef PREFERENCE_H
#define PREFERENCE_H

#include <cstdlib>
#include <ctime>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include <assert.h>
#include <math.h>

using namespace std;

const unsigned int PREFERENCE_MAX_DIM = 20;

class Preference
{
    public:
        Preference()
        {
            srand((unsigned)time(0));
        }

        void prepareUniData(unsigned int total, unsigned int dim);
        void prepareClusteredData(unsigned int total, unsigned int dim);

    protected:
    private:
        double getRandom();

        double normalDistribution(double x, double mu, double delta);
};

inline double Preference::getRandom()
{
    double num = rand();
    return num / RAND_MAX;
}

inline double Preference::normalDistribution(double x, double mu, double delta)
{
    double coe = 1 / delta * sqrt(2 * M_PI);
    double exponent = (x - mu) * (mu - x) / (2 * delta * delta);
    return coe * exp(exponent);
}


#endif // PREFERENCE_H
