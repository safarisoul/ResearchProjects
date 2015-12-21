#ifndef COLOR_H
#define COLOR_H

#include <cstdlib>
#include <ctime>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include <assert.h>

#define COLOR_ENTRY_RAW_FILE  "raw/ColorMoments.asc"

using namespace std;

const int COLOR_ENTRY_DIM = 9;

typedef struct ColorEntry
{
    friend istream& operator >> (istream& in, ColorEntry& c);
    friend ostream& operator << (ostream& out, ColorEntry& c);

    double value[COLOR_ENTRY_DIM];
}ColorEntry;

class Color
{
    public:
        Color()
        {
            srand((unsigned)time(0));
        }

        void prepareData(double minValue, double maxValue, unsigned int total);

    protected:

    private:
        vector<ColorEntry> entries;
        double minValue[COLOR_ENTRY_DIM];
        double maxValue[COLOR_ENTRY_DIM];

        void readRawData();
        void disturb();
        void normalize(double normMin, double normMax);
        void writeData(double normMin, double normMax, unsigned int total);

        double getRandom();
};

inline double Color::getRandom()
{
    double num = rand();
    return num / RAND_MAX;
}

#endif // COLOR_H
