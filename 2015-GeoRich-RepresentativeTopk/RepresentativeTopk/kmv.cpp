#include "kmv.h"

int KMV::size = 17;
double KMV::goldenratio = (1 + sqrt(5)) * 0.5;

Heap KMV::getkmv(set<int>& data)
{
    Heap kmv;
    set<int>::iterator is;
    for(is=data.begin(); is!=data.end(); is++)
    {
        double h = hash(*is + 1);
        if(kmv.size() < size)
            kmv.push(HeapEntry(h, *is));
        else
        {
            if(h < kmv.top())
            {
                kmv.pop();
                kmv.push(HeapEntry(h, *is));
            }
        }
    }
    return kmv;
}

void KMV::combine(Heap& to, Heap& from)
{
    if(to.isEmpty())
    {
        for(int is=0; is<from.size(); is++)
            to.push(from.hev[is]);
        while(to.size() > size)
            to.pop();
    }
    else
    {
        int nsize = min(to.size(), from.size());
        while(to.size() > nsize)
            to.pop();
        for(int is=0; is<from.size(); is++)
            if(from.hev[is].key < to.top() && !to.haskey(from.hev[is].key))
            {
                to.pop();
                to.push(from.hev[is]);
            }
    }
}

void KMV::combine(Heap& kmv, Heap& kmv1, Heap& kmv2)
{
    if(kmv1.size() == 0)
    {
        for(int is=0; is<kmv2.size(); is++)
            kmv.push(kmv2.hev[is]);
        return;
    }

    if(kmv2.size() == 0)
    {
        for(int is=0; is<kmv1.size(); is++)
            kmv.push(kmv1.hev[is]);
        return;
    }

    int nsize = min(kmv1.size(), kmv2.size());
    for(int is=0; is<kmv1.size(); is++)
        kmv.push(kmv1.hev[is]);
    // kmv1.size() >= nsize  ==>  kmv.size() >= nsize
    while(kmv.size() > nsize)
        kmv.pop();
    for(int is=0; is<kmv2.size(); is++)
        if(kmv2.hev[is].key < kmv.top() && !kmv.haskey(kmv2.hev[is].key))
        {
            kmv.pop();
            kmv.push(kmv2.hev[is]);
        }
}
