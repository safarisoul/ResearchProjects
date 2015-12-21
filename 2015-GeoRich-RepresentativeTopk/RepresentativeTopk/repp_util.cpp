#include "repp_util.h"

void Score::output()
{
    cout << endl;
    cout << "distance score : " << dscore << endl;
    cout << "coverage score : " << cscore << endl;
    cout << "weighted score : " << score << endl;
}

void REPP_UTIL::pick(set<int>& rst, Score& s, map<int, set<int> >& rtks, int wsize, int t, double dweight)
{
    Record::candidate_cnt = rtks.size();

    struct timeval start, end;
    gettimeofday(&start, NULL);

    set<int> cov;
    double mind = 1;
    for(int i=0; i<t; i++)
    {
        double best = -1;
        double bestdscore = -1;
        int bestid = -1;
        map< int, set<int> >::iterator im;
        for(im = rtks.begin(); im != rtks.end(); im++)
            if(rst.find(im->first) == rst.end())
            {
                double dscore = min(mind, mindis(rtks, rst, im->first));
                int total = coverage(cov, im->second);
                double cscore = total * 1.0 / wsize;
                double score = dweight * dscore + (1 - dweight) * cscore;
                if(score > best)
                {
                    best = score;
                    bestdscore = dscore;
                    bestid = im->first;
                }
            }
        if(bestid != -1)
        {
            mind = bestdscore;
            setunion(cov, rtks[bestid]);
            rst.insert(bestid);
            double dscore = mind;
            double cscore = cov.size() * 1.0 / wsize;
            double score = dweight * dscore + (1 - dweight) * cscore;
            s.cscore = cscore;
            s.dscore = dscore;
            s.score = score;
        }
    }

    gettimeofday(&end, NULL);
    Record::pick_time = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) * 1e-6;
}

void REPP_UTIL::pick_mh(set<int>& rst, Score& s, map<int, Heap>& kmvm, map< int, vector<int> >& mhm, int wsize, int t, double dweight)
{
    Record::candidate_cnt = kmvm.size();

    struct timeval start, end;
    gettimeofday(&start, NULL);

    Heap cov;
    double mind = 1;
    for(int i=0; i<t; i++)
    {
        double bestscore = -1, bestdscore = -1, bestcscore = -1;
        int bestid = -1;
        map<int, Heap>::iterator im;
        for(im = kmvm.begin(); im != kmvm.end(); im++)
            if(rst.find(im->first) == rst.end())
            {
                double dscore = min(mind, mindis(mhm, rst, im->first));
                double total = KMV::unionsize(cov, im->second);
                double cscore = total / wsize;
                //double cscore = min(total / wsize, 1.0);
                double score = dweight * dscore + (1 - dweight) * cscore;
                if(score > bestscore)
                {
                    bestscore = score;
                    bestdscore = dscore;
                    bestcscore = cscore;
                    bestid = im->first;
                }
            }
        if(bestid != -1)
        {
            KMV::combine(cov, kmvm[bestid]);
            rst.insert(bestid);
            s.cscore = bestcscore;
            s.dscore = bestdscore;
            s.score = bestscore;
        }
    }

    gettimeofday(&end, NULL);
    Record::pick_mh_time = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) * 1e-6;
}

void REPP_UTIL::pick_mh2(set<int>& rst, Score& s, map<int, Heap>& kmvm, map< int, vector<int> >& mhm, int wsize, int t, double dweight)
{
    Record::candidate_cnt = kmvm.size();

    struct timeval start, end;
    gettimeofday(&start, NULL);

    Heap cov;
    double mind = 1;
    for(int i=0; i<t; i++)
    {
        double bestscore = -1, bestdscore = -1, bestcscore = -1;
        int bestid = -1;
        map<int, Heap>::iterator im;
        for(im = kmvm.begin(); im != kmvm.end(); im++)
            if(rst.find(im->first) == rst.end())
            {
                double dscore = min(mind, mindis(mhm, rst, im->first));
                double total = KMV::unionsize(cov, im->second);
                double cscore = total / wsize;
                //double cscore = min(total / wsize, 1.0);
                double score;
                if(i == 0)
                    score = cscore;
                else if(i == 1)
                    score = dscore;
                else
                    score = dweight * dscore + (1 - dweight) * cscore;
                if(score > bestscore)
                {
                    bestscore = score;
                    bestdscore = dscore;
                    bestcscore = cscore;
                    bestid = im->first;
                }
            }
        if(bestid != -1)
        {
            KMV::combine(cov, kmvm[bestid]);
            rst.insert(bestid);
            s.cscore = bestcscore;
            s.dscore = bestdscore;
            s.score = bestscore;
        }
    }

    gettimeofday(&end, NULL);
    Record::pick_mh_time = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) * 1e-6;
}

void REPP_UTIL::mhkmv(map<int, Heap>& kmvm, map<int, vector<int> >& mhm, map<int, set<int> >& rtks)
{
    struct timeval start, end;
    gettimeofday(&start, NULL);

    map<int, set<int> >::iterator im;
    for(im = rtks.begin(); im != rtks.end(); im++)
    {
        kmvm[im->first] = KMV::getkmv(im->second);
        mhm[im->first] = MinHash::getmh(im->second);
    }

    gettimeofday(&end, NULL);
    Record::overhead_time = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) * 1e-6;
}

void REPP_UTIL::score(set<int>& rst, Score& s, map<int, set<int> >& rtks, int wsize, int t, double dweight)
{
    set<int> cov;
    double mind = 1;
    set<int>::iterator ir;
    for(ir = rst.begin(); ir != rst.end(); ir++)
    {
        mind = min(mind, mindis(rtks, rst, *ir));
        setunion(cov, rtks[*ir]);
    }
    s.dscore = mind;
    s.cscore = cov.size() * 1.0 / wsize;
    s.score = dweight * s.dscore + (1 - dweight) * s.cscore;
}

double REPP_UTIL::mindis(map<int, set<int> >& rtks, set<int>& rst, int id)
{
    if(rst.size() == 0)
        return 1;

    double mind = 1;
    set<int>::iterator is;
    for(is = rst.begin(); is != rst.end(); is++)
        if(*is != id)
            mind = min(mind, 1 - jaccardsimilarity(rtks[*is], rtks[id]));

    return mind;
}

double REPP_UTIL::jaccardsimilarity(set<int>& set1, set<int>& set2)
{
    int cnt = 0;
    set<int>::iterator is;
    for(is = set1.begin(); is != set1.end(); is++)
        if(set2.find(*is) != set2.end())
            cnt++;
    return cnt * 1.0 / (set1.size() + set2.size() - cnt);
}

int REPP_UTIL::coverage(set<int>& set1, set<int>& set2)
{
    int cnt = 0;
    set<int>::iterator is;
    for(is = set2.begin(); is != set2.end(); is++)
        if(set1.find(*is) != set1.end())
            cnt++;
    return set1.size() + set2.size() - cnt;
}

void REPP_UTIL::setunion(set<int>& to, set<int>& from)
{
    set<int>::iterator is;
    for(is = from.begin(); is != from.end(); is++)
        to.insert(*is);
}

double REPP_UTIL::mindis(map< int, vector<int> >& mhm, set<int>& rst, int id)
{
    if(rst.size() == 0)
        return 1;

    double mind = 1;
    set<int>::iterator is;
    for(is = rst.begin(); is != rst.end(); is++)
        if(*is != id)
            mind = min(mind, 1 - MinHash::jaccardsimilarity(mhm[*is], mhm[id]));

    return mind;
}
