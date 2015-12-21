#include "minhash.h"

int MinHash::nfun = 100;
int MinHash::hfun[MAX_FUN][3] = {{0,0,0}};
int MinHash::PRIME_MAX = 10000000;
int MinHash::PRIME_MIN = 1000000;

void MinHash::init(int nf)
{
    assert(nf <= MAX_FUN);
    nfun = nf;

    vector<int> primes;
    vector<bool> prime;
    for(int i=0; i<PRIME_MAX; i++)
        prime.push_back(true);
    for(int i=2; i<PRIME_MAX; i++)
        if(prime[i])
        {
            if(i > PRIME_MIN)
                primes.push_back(i);
            for(int j=i+i; j<PRIME_MAX; j+=i)
                prime[j] = false;
        }

    int index;
    for(int i=0; i<nfun; i++)
    {
        index = getRandom(primes.size());
        hfun[i][0] = primes[index];
        index = getRandom(primes.size());
        hfun[i][1] = primes[index];
        index = getRandom(primes.size());
        hfun[i][2] = primes[index];
    }

}

vector<int> MinHash::getmh(set<int>& data)
{
    vector<int> mh;
    for(int i=0; i<nfun; i++)
        mh.push_back(INT_MAX);

    set<int>::iterator is;
    for(is = data.begin(); is != data.end(); is++)
        for(int i=0; i<nfun; i++)
            mh[i] = min(mh[i], hash(*is, i));

    return mh;
}

double MinHash::jaccardsimilarity(vector<int>& mh1, vector<int>& mh2)
{
    int cnt = 0;
    for(int i=0; i<nfun; i++)
        if(mh1[i] != INT_MAX && mh1[i] == mh2[i])
            cnt++;
    return cnt / (double)nfun;
}

double MinHash::jaccardsimilarity(vector< vector<int> >& mhs)
{
    assert(mhs.size() > 0);
    int cnt = 0;
    for(int i=0; i<nfun; i++)
    {
        int min = mhs[0][i];
        bool equal = true;
        for(unsigned int j=1; j<mhs.size(); j++)
            if(mhs[j][i] != min)
            {
                equal = false;
                break;
            }
        if(equal && min != INT_MAX)
            cnt++;
    }
    return cnt / (double)nfun;
}
