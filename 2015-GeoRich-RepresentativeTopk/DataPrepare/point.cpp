#include "point.h"

/**
 * S. B¨orzs¨onyi, D. Kossmann, and K. Stocker. The skyline operator. In Proc.
 * of ICDE, pages 421–430, 2001.
 *
 * First we select a plane perpendicular to the line from (0, . . . , 0) to
 * (maxValue, . . . , maxValue) using a normal distribution; the new point will
 * be in that plane. We use a normal distribution to select the plane so that
 * more points are in the middle than at the ends. Within the plane, the
 * individual attribute values are again generated using a normal distribution;
 * this makes sure that most points are located close to the line from
 * (0, . . . 0) to (maxValue, . . . maxValue).
 */
void Point::prepareCorrelatedData(double maxValue, unsigned int total, unsigned int dim)
{
    assert(dim <= POINT_MAX_DIM);
    stringstream sstream;
    sstream << "data/point_correlated_" << total << "_" << dim << ".txt";
    ofstream pointOFS(sstream.str().c_str());
    assert(pointOFS.good());
    double mu = 0, delta = sqrt(0.5);
    for(unsigned int i=0; i<total; i++)
    {
        double planeloc = ndpoint(0, maxValue, mu, delta, 1);
        double coord[POINT_MAX_DIM];
        while(true)
        {
            double sum = 0;
            bool ok = true;
            for(unsigned int d=1; d<dim; d++)
            {
                if(planeloc * 2 <= maxValue)
                    coord[d] = ndpoint(0, 2 * planeloc, mu, delta, 5);
                else
                    coord[d] = ndpoint(2 * planeloc - maxValue, maxValue, mu, delta, 5);

                if(coord[d] > maxValue)
                {
                    ok = false;
                    break;
                }
                sum += coord[d];
            }
            if(!ok)
                continue;
            coord[0] = dim * planeloc - sum;
            if(coord[0] > 0 && coord[0] < 2 * planeloc && coord[0] < maxValue)
                break;
        }

        for(unsigned int d=0; d<dim; d++)
        {
            if(d>0)
                pointOFS << " ";
            pointOFS << coord[d];
        }
        pointOFS << endl;
    }
    pointOFS.close();
}

/**
 * S. B¨orzs¨onyi, D. Kossmann, and K. Stocker. The skyline operator. In Proc.
 * of ICDE, pages 421–430, 2001.
 *
 * As for a correlated database, we generate a point by first selecting a plane
 * perpendicular to the line from (0, . . . , 0) to (maxValue, . . . , maxValue)
 * using a normal distribution. We use a normal distribution with very small
 * variance so that all points are placed into planes which are close to the
 * plane through the point (maxValue * 0.5, . . . , maxValue * 0.5). Within the
 * plane, the individual attribute values are generated using a uniform
 * distribution.
 */
void Point::prepareAntiCorrelatedData(double maxValue, unsigned int total, unsigned int dim)
{
    assert(dim <= POINT_MAX_DIM);
    stringstream sstream;
    sstream << "data/point_anticorrelated_" << total << "_" << dim << ".txt";
    ofstream pointOFS(sstream.str().c_str());
    assert(pointOFS.good());
    double mu = 0, delta = sqrt(0.5);
    for(unsigned int i=0; i<total; i++)
    {
        double planeloc = ndpoint(0, maxValue, mu, delta, 10);
        double coord[POINT_MAX_DIM];
        while(true)
        {
            double sum = 0;
            bool ok = true;
            for(unsigned int d=1; d<dim; d++)
            {
                if(planeloc * 2 <= maxValue)
                    coord[d] = udpoint(0, 2 * planeloc);
                else
                    coord[d] = udpoint(2 * planeloc - maxValue, maxValue);

                if(coord[d] > maxValue)
                {
                    ok = false;
                    break;
                }
                sum += coord[d];
            }
            if(!ok)
                continue;
            coord[0] = dim * planeloc - sum;
            if(coord[0] > 0 && coord[0] < 2 * planeloc && coord[0] < maxValue)
                break;
        }

        for(unsigned int d=0; d<dim; d++)
        {
            if(d>0)
                pointOFS << " ";
            pointOFS << coord[d];
        }
        pointOFS << endl;
    }
    pointOFS.close();
}

/**
 * Orestis Gkorgkas, Akrivi Vlachou, Christos Doulkeridis, Kjetil Nørvåg:
 * Finding the Most Diverse Products using Preference Queries. EDBT 2015: 205-216
 *
 * First, 5 cluster centroids were selected randomly. Then, each coordinate is
 * generated on the m-dimensional space by following a normal distribution on
 * each axis with variance 0.345, and a mean equal to the corresponding
 * coordinate of the centroid.
 */
void Point::prepareClusteredData(double maxValue, unsigned int total, unsigned int dim)
{
    assert(dim <= POINT_MAX_DIM);
    stringstream sstream;
    sstream << "data/point_clustered_" << total << "_" << dim << ".txt";
    ofstream pointOFS(sstream.str().c_str());
    assert(pointOFS.good());
    double delta = sqrt(0.345);

    double center[POINT_MAX_DIM * 2][POINT_MAX_DIM];
    double range[POINT_MAX_DIM * 2];
    for(int i=0; i<dim*2; i++)
    {
        for(unsigned int d=0; d<dim; d++)
            center[i][d] = getRandom() * maxValue;
        range[i] = maxValue * 0.2 + getRandom() * maxValue * 0.2;
    }

    for(unsigned int i=0; i<total; i++)
    {
        int c = i % (dim * 2);
        double coord[POINT_MAX_DIM];
        for(unsigned int d=0; d<dim; d++)
        {
            double fullmark = normalDistribution(0, 0, delta);
            while(true)
            {
                double x = getRandom() * 8 - 4;
                double passmark = normalDistribution(x, 0, delta);
                if(getRandom() < passmark / fullmark)
                {
                    coord[d] = center[c][d] + x * range[c] / 4;
                    if(coord[d]>0 && coord[d]<maxValue)
                        break;
                }
            }
            if(d>0)
                pointOFS << " ";
            pointOFS << coord[d];
        }
        pointOFS << endl;
    }
    pointOFS.close();
}

void Point::prepareUniData(double maxValue, unsigned int total, unsigned int dim)
{
    assert(dim <= POINT_MAX_DIM);
    stringstream sstream;
    sstream << "data/point_uni_" << total << "_" << dim << ".txt";
    ofstream pointOFS(sstream.str().c_str());
    assert(pointOFS.good());

    for(unsigned int i=0; i<total; i++)
    {
        double coord[POINT_MAX_DIM];
        for(unsigned int d=0; d<dim; d++)
        {
            coord[d] = maxValue * getRandom();
            if(d>0)
                pointOFS << " ";
            pointOFS << coord[d];
        }
        pointOFS << endl;
    }
    pointOFS.close();
}
