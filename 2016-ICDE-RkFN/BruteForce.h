#ifndef BRUTEFORCE_H
#define BRUTEFORCE_H

#include "RStarTreeUtil.h"

class BruteForce
{
    public:
        BruteForce();

        static void print(Point_V& results);
        static void rkfn(Point_V& results);

    private:
        static bool verify(Point& client);
};

#endif // BRUTEFORCE_H
