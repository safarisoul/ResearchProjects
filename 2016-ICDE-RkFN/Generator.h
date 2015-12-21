#ifndef GENERATOR_H
#define GENERATOR_H

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <limits>
#include <algorithm>
#include <vector>

#include "DCCR.h"

#define INF_N -numeric_limits<double>::max()
#define INF_P numeric_limits<double>::max()

#define CAL_NODE_FILE "data/real/raw/cal.cnode.txt"
#define NUM_CAL_NODES 21048
#define CAL_POINT_FILE "data/real/raw/caldata.txt"
#define NUM_CAL_POINTS 104770
#define NA_NODE_FILE "data/real/raw/NA.cnode.txt"
#define NUM_NA_NODES 175813
#define SF_NODE_FILE "data/real/raw/SF.cnode.txt"
#define NUM_SF_NODES 174956

#define REAL_DATA_FILE "data/real/point.txt"
#define NUM_POINTS 474655

#define UNI_DATA_FILE "data/uni/point.txt"

#define UNIC_DATA_FILE "data/unic/point.txt"

using namespace std;

typedef struct FSUNode
{
    friend istream& operator>>(istream& in, FSUNode& n);

    bool operator< (const FSUNode& n) const;

    double longitude;
    double latitude;
}FSUNode;

typedef struct FSUPoint
{
    friend istream& operator>>(istream& in, FSUPoint& p);

    bool operator< (const FSUPoint& p) const;

    double longitude;
    double latitude;
}FSUPoint;

typedef struct Point2D
{
    Point2D();
    Point2D(double x, double y);

    friend istream& operator>>(istream& in, Point2D& p);
    friend ostream& operator<<(ostream& out, const Point2D& p);

    bool operator< (const Point2D& p) const;

    double x;
    double y;
}Point2D;

class RealDataGenerator
{
    public:
        RealDataGenerator();

        // real data
        void genRealData();
        void pickFC(size_t nf, size_t nc, size_t nq);
        void rstQ();
        void rstQ1();

        // uniform data
        void genUniData(size_t total);
        void pickUni(size_t nf, size_t nc, size_t nq);

        // uniform data in a circle
        void genUniCData(size_t total);
        void pickUniC(size_t nf, size_t nc, size_t nq);
        void vdca();

    protected:

    private:
        vector<FSUNode> calNodes;
        vector<FSUPoint> calPoints;
        vector<FSUNode> naNodes;
        vector<FSUNode> sfNodes;

        vector<Point2D> points;

        vector<Point2D> facilities;
        vector<Point2D> clients;
        vector<Point2D> queries;

        // real data
        void readCalNode();
        void readCalPoint();
        void readNANode();
        void readSFNode();
        template <class T>
        void normalize(vector<T> &pointV);
        template <class T>
        void duplicateDetect(vector<T> &pointV);
        template <class T>
        void duplicateRemove(vector<T> &pointV);

        void check();
        void writePoints(ofstream& out, vector<Point2D>& source, string fileName);

        void writeRealData();
        void readRealData();

        void pick(vector<Point2D>& from, vector<Point2D>& to, vector<Point2D>& left, size_t num);

        // uniform data
        void writeUniData();
        void readUniData();

        // uniform data in a circle
        void writeUniCData();
        void readUniCData();

        // inline functions
        double getRandom(); // [0, 1]
};

inline double RealDataGenerator::getRandom()
{
    double num = rand();
    return num / RAND_MAX;
}

#endif // GENERATOR_H
