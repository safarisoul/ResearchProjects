#include "preference.h"

void Preference::prepareUniData(unsigned int total, unsigned int dim)
{
    assert(dim <= PREFERENCE_MAX_DIM);
    stringstream sstream;
    sstream << "data/preference_uni_" << total << "_" << dim << ".txt";
    ofstream preferenceOFS(sstream.str().c_str());
    assert(preferenceOFS.good());
    for(unsigned int i=0; i<total; i++)
    {
        double weights[PREFERENCE_MAX_DIM];
        double sum = 0;
        for(unsigned int d=0; d<dim; d++)
        {
            weights[d] = getRandom();
            sum += weights[d];
        }
        for(unsigned int d=0; d<dim; d++)
        {
            weights[d] = weights[d] / sum;
            if(d>0)
                preferenceOFS << " ";
            preferenceOFS << weights[d];
        }
        preferenceOFS << endl;
    }
    preferenceOFS.close();
}

void Preference::prepareClusteredData(unsigned int total, unsigned int dim)
{
    assert(dim <= PREFERENCE_MAX_DIM);
    stringstream sstream;
    sstream << "data/preference_clustered_" << total << "_" << dim << ".txt";
    ofstream preferenceOFS(sstream.str().c_str());
    assert(preferenceOFS.good());
    double delta = sqrt(0.345);

    double center[PREFERENCE_MAX_DIM * 2][PREFERENCE_MAX_DIM];
    double range[PREFERENCE_MAX_DIM * 2];
    for(int i=0; i<dim*5; i++)
    {
        double sum = 0;
        for(unsigned int d=0; d<dim; d++)
        {
            center[i][d] = getRandom();
            sum += center[i][d];
        }
        for(unsigned int d=0; d<dim; d++)
            center[i][d] /= sum;
        range[i] = 0.05 + getRandom() * 0.05;
    }

    for(unsigned int i=0; i<total; i++)
    {
        int c = i % (dim * 5);
        double coord[PREFERENCE_MAX_DIM];
        double sum = 0;
        double fullmark = normalDistribution(0, 0, delta);
        for(unsigned int d=0; d<dim; d++)
            while(true)
            {
                double x = getRandom() * 8 - 4;
                double passmark = normalDistribution(x, 0, delta);
                if(getRandom() < passmark / fullmark)
                {
                    coord[d] = center[c][d] + x * range[c] / 4;
                    sum += coord[d];
                    break;
                }
            }
        for(unsigned int d=0; d<dim; d++)
        {
            coord[d] /= sum;
            if(d>0)
                preferenceOFS << " ";
            preferenceOFS << coord[d];
        }
        preferenceOFS << endl;
    }
    preferenceOFS.close();
}
