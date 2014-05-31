#include "main.h"

using namespace std;

int main(int argc, char* argv[])
{
    cout << endl;
    // read argument
    Argument::readArguments(argc, argv);
    Argument::printArguments();

    // load data
    if(Argument::network == "california")
    {
        CaliforniaDataLoader loader;
        loader.load();
        loader.checkData();
    }
    else if(Argument::network == "northAmerica")
    {
        NorthAmericaDataLoader loader;
        loader.load();
        loader.checkData();
    }

    // run
    if(Argument::function == "genq")
    {
        QueryGenerator generator;
        generator.run();
        Data::clean();
    }
    else if(Argument::function == "geno")
    {
        ObjectGenerator generator;
        generator.run();
        // Data::clean(); not sure why this cause error, memory leaks present
    }
    else if(Argument::function == "genm")
    {
        if(Argument::network == "northAmerica")
        {
            NorthAmericaDataLoader loader;
            loader.generateMap();
        }
        Data::clean();
    }
    else if(Argument::function == "m")
    {
        Monitor monitor;
        monitor.zoneForAll();
        monitor.monitor();
        Argument::outputExperiment();
        Data::clean();
    }
    else if(Argument::function == "mu")
    {
        Monitor monitor;
        monitor.zoneForAll();
        monitor.update();
        Argument::outputExperiment();
        Data::clean();
    }
    else if(Argument::function == "sac")
    {
        SAC sac;
        sac.computeUnprunedNetworkForAll();
        sac.monitor();
        Argument::outputExperiment();
        Data::clean();
    }
    else if(Argument::function == "u")
    {
        Update monitor;
        monitor.zoneForAll();
        monitor.update();
        Argument::outputExperiment();
        Data::clean();
    }
    else if(Argument::function == "um")
    {
        Update monitor;
        monitor.zoneForAll();
        monitor.monitor();
        Argument::outputExperiment();
        Data::clean();
    }
    else if(Argument::function == "us")
    {
        Update monitor;
        monitor.zoneForAll();
        monitor.updateSimple();
        Argument::outputExperiment();
        Data::clean();
    }
}
