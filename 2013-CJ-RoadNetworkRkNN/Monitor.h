#ifndef MONITOR_H
#define MONITOR_H

#include "Zone.h"

class Monitor
{
    public:
        Monitor();
        void zoneForAll();
        void monitor();
        void update();

    protected:

    private:
        PIDSet_t kQueries;
        ObjectV_t objects;
        PIDV_t updates;

        string objectFileName;
        ifstream objectFile;

        string queryFileName;
        ifstream queryFile;

        inline int getRandom(int max) { return rand() % max; }; // [0, max)
        inline double getRandom() { double num = rand() * 1e-9; return num - (int) num; }; // (0, 1)
};

#endif // MONITOR_H
