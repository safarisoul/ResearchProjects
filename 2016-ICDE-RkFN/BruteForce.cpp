#include "BruteForce.h"

BruteForce::BruteForce()
{
}

void BruteForce::print(Point_V& results)
{
    sort(results.begin(), results.end());
    for(size_t ir = 0; ir < results.size(); ir++)
        results.at(ir).print();
}

void BruteForce::rkfn(Point_V& results)
{
    results.clear();
    Point_V& clients = Data::clients;
    for(size_t ic = 0; ic < clients.size(); ic++)
        if(verify(clients.at(ic)))
            results.push_back(clients.at(ic));
}

bool BruteForce::verify(Point& client)
{
    double r = client.distance2(*Data::query);
    return RStarTreeUtil::rangeQuery2(*Data::fTree, client, r, Argument::k) < Argument::k;
}
