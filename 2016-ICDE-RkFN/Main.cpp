#include "Main.h"

int main(int argc, char* argv[])
{
    Argument::readArguments(argc, argv);

    //init();

    run();

    return EXIT_SUCCESS;
}

void run()
{
    Argument::printArguments();

    if(Argument::gen == "y")
    {
        if(Argument::dataset == "real")
            genRealData();
        else if(Argument::dataset == "uni")
            genUniData();
        else if(Argument::dataset == "unic")
            genUniCData();
    }

    if(Argument::pick == "y")
    {
        if(Argument::dataset == "real")
            pickFCFromRealData();
        else if(Argument::dataset == "uni")
            pickFCFromUniData();
        else if(Argument::dataset == "unic")
            pickFCFromUniCData();
    }

    loadData();

    if(Argument::rstq == "y")
    {
        if(Argument::pick == "y")
            genrstq();
        loadData();
    }
    else if(Argument::rstq1 == "y")
    {
        if(Argument::pick == "y")
            genrstq1();
        loadData();
    }

    if(Argument::vdca == "y")
        vdca();

    size_t T = 0, count = 0;
    while(Data::query != NULL)
    {
        T++;
        size_t cnt = 0, num1 = 0, num2 = 0, num3 = 0;

        if(Argument::bf == "y")
        {
            num1 = runBruteForce();
            if(num1 > 0)
                cnt += 1;
        }
        if(Argument::chfc == "y")
        {
            num2 = runCHFC();
            if(num2 > 0)
                cnt += 2;
        }
        if(Argument::dccr == "y")
        {
            num3 = runRkFN();
            if(num3 > 0)
                cnt += 4;
        }
        if((Argument::bf == "y" && Argument::chfc == "y" && num1 != num2) || (Argument::bf == "y" && Argument::dccr == "y" && num1 != num3) || (Argument::chfc == "y" && Argument::dccr == "y" && num2 != num3))
        {
            cout << "(" << num1 << " " << num2 << " " << num3 << ")" << endl;
            count++;
        }
        else
        {
            if(cnt == 0)
            {
                cout << "." << flush;
                Argument::empty++;
            }
            else
                cout << "=" << flush;
        }

        if(Argument::vara == "y")
            vara();

        if(Argument::tpru == "y")
            tpru();
        if(T % 10 == 0)
            cout << " " << flush;
        if(T % 50 == 0)
            cout << T << endl;

        Data::next();
    }

    cout << endl << count << " pairs" << endl;

    Argument::outputExperiment();
    Argument::clear();
}

void init()
{
    ofstream debug;
    debug.open ("data/debug/debug.txt", ios::out | ios::trunc );
    if (debug.is_open())
    {
        debug << "k = " << Argument::k << endl;
        debug << "nf = " << Argument::numFacilities << endl;
        debug << "nc = " << Argument::numClients << endl;
        debug << "EPS = " << EPS << endl;
        debug.close();
    }
    else
        assert(false);

    ofstream log;
    log.open ("data/debug/log.txt", ios::out | ios::trunc );
    if (log.is_open())
    {
        log << "k = " << Argument::k << endl;
        log << "nf = " << Argument::numFacilities << endl;
        log << "nc = " << Argument::numClients << endl;
        log << "EPS = " << EPS << endl;
        log.close();
    }
    else
        assert(false);

    ofstream error;
    error.open ("data/debug/error.txt", ios::out | ios::trunc );
    if (error.is_open())
    {
        error << "k = " << Argument::k << endl;
        error << "nf = " << Argument::numFacilities << endl;
        error << "nc = " << Argument::numClients << endl;
        error << "EPS = " << EPS << endl;
        error.close();
    }
    else
        assert(false);

    ofstream chfc;
    chfc.open ("data/debug/chfc.txt", ios::out | ios::trunc );
    if (chfc.is_open())
    {
        chfc << "k = " << Argument::k << endl;
        chfc << "nf = " << Argument::numFacilities << endl;
        chfc << "nc = " << Argument::numClients << endl;
        chfc << "EPS = " << EPS << endl;
        chfc.close();
    }
    else
        assert(false);

    ofstream fn;
    fn.open ("data/debug/fn.txt", ios::out | ios::trunc );
    if (fn.is_open())
        ;
    else
        assert(false);
}

void loadData()
{
    Data::buildFTree();
    Data::buildCTree();
    Data::loadQueries();

    //RTreeUtil::validate(*Data::fTree);

    //Point q(99999.9, 99999.9);
    //Data::query = &q;
}

void genRealData()
{
    RealDataGenerator gen;
    gen.genRealData();
}

void genUniData()
{
    RealDataGenerator gen;
    gen.genUniData(3000000);
}

void genUniCData()
{
    RealDataGenerator gen;
    gen.genUniCData(3000000);
}

void pickFCFromRealData()
{
    RealDataGenerator gen;
    gen.pickFC(Argument::numFacilities, Argument::numClients, Argument::numQueries);
}

void pickFCFromUniData()
{
    RealDataGenerator gen;
    gen.pickUni(Argument::numFacilities, Argument::numClients, Argument::numQueries);
}

void pickFCFromUniCData()
{
    RealDataGenerator gen;
    gen.pickUniC(Argument::numFacilities, Argument::numClients, Argument::numQueries);
}

size_t runBruteForce()
{
    struct timeval start, end;
    Point_V results;

    gettimeofday(&start, NULL);
    BruteForce::rkfn(results);
    gettimeofday(&end, NULL);

    long seconds = end.tv_sec  - start.tv_sec;
    long useconds = end.tv_usec - start.tv_usec;
    Argument::Tbf += seconds * 1e6 + useconds;

    //cout << "result " << results.size() << endl;
    //BruteForce::print(results);

    return results.size();
}

size_t runCHFC()
{
    struct timeval start, end;
    Result result, notResult;

    gettimeofday(&start, NULL);
    CHFC::chfc(*Data::fTree, *Data::cTree, *Data::query, Argument::k, result, notResult);
    gettimeofday(&end, NULL);

    long seconds = end.tv_sec  - start.tv_sec;
    long useconds = end.tv_usec - start.tv_usec;
    Argument::Tchfc += seconds * 1e6 + useconds;

    return result.aggregate;
}

size_t runRkFN()
{
    struct timeval start, end;
    Result result, notResult;

    gettimeofday(&start, NULL);
    DCCR::rkfn(*Data::fTree, *Data::cTree, *Data::query, Argument::k, result, notResult);
    gettimeofday(&end, NULL);

    long seconds = end.tv_sec  - start.tv_sec;
    long useconds = end.tv_usec - start.tv_usec;
    Argument::Tdccr += seconds * 1e6 + useconds;

    return result.aggregate;
}

void genrstq()
{
    RealDataGenerator gen;
    gen.rstQ();
    Argument::clear();
}

void genrstq1()
{
    RealDataGenerator gen;
    gen.rstQ1();
    Argument::clear();
}

void vdca()
{
    RealDataGenerator gen;
    gen.vdca();
}

void vara()
{
    Result result, notResult;
    DCCR::vara(*Data::fTree, *Data::cTree, *Data::query, Argument::k, result, notResult);
}

void tpru()
{
    Result result, notResult;
    DCCR::tpru(*Data::fTree, *Data::cTree, *Data::query, Argument::k, result, notResult);
}







