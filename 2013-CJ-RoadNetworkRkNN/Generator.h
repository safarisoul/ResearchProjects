#ifndef GENERATOR_H
#define GENERATOR_H

#include <cstdlib>
#include <fstream>

#include "Data.h"

class QueryGenerator
{
    public:
        QueryGenerator();

        void run();
        inline int getRandom(int max) { return rand() % max; }; // [0, max)
        inline double getRandom() { double num = rand() * 1e-9; return num - (int) num; }; // (0, 1)

    protected:

    private:
        string queryFileName;
        ofstream queryFile;

};

class ObjectGenerator
{
    public:
        ObjectGenerator();

        void run();
        inline int getRandom(int max) { return rand() % max; }; // [0, max)
        inline double getRandom() { double num = rand() * 1e-9; return num - (int) num; }; // (0, 1)

    protected:

    private:
        ObjectV_t objects;

        string objectFileName;
        ofstream objectFile;

        Distance_t total, min, max, avg;

};

#endif // GENERATOR_H
