#include <base/ArgumentParser.h>
#include <framework/TestRunner.h>

#include <iostream>

int main(int argc, char* argv[])
{
    try {
        base::ArgumentParser argParser{ argc, argv };
        framework::TestRunner testRunner{ std::move(argParser) };

        testRunner.run();

    } catch (std::exception exception) {
        std::cerr << "Caught std::exception!" << std::endl;
        std::cerr << "Message: " << exception.what() << std::endl;
        return -1;

    } catch (...) {
        std::cerr << "Caught unknown exception!" << std::endl;
        return -1;
    }

    return 0;
}
