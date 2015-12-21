#ifndef HOUSE_H
#define HOUSE_H

#include <cstdlib>
#include <ctime>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include <assert.h>

#define HOUSE_ENTRY_RAW_FILE  "raw/usa_00003.csv"

using namespace std;

const int HOUSE_ENTRY_DIM = 6;

typedef struct HouseEntry
{
    friend istream& operator >> (istream& in, HouseEntry& h);
    friend ostream& operator << (ostream& out, HouseEntry& h);

    double value[HOUSE_ENTRY_DIM];
}HouseEntry;

class House
{
    public:
        House()
        {
            srand((unsigned)time(0));
        }

        void prepareData(double minValue, double maxValue, unsigned int total);

    protected:

    private:
        vector<HouseEntry> entries;
        double minValue[HOUSE_ENTRY_DIM];
        double maxValue[HOUSE_ENTRY_DIM];

        void readRawData();
        void disturb();
        void normalize(double normMin, double normMax);
        void writeData(double normMin, double normMax, unsigned int total);

        double getRandom();
};

inline double House::getRandom()
{
    double num = rand();
    return num / RAND_MAX;
}

#endif // HOUSE_H
