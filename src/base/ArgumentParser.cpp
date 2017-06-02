#include <base/ArgumentParser.h>

#include <vector>

namespace base {
ArgumentParser::ArgumentParser(int argc, char* argv[])
    : path(argv[0])
{
    std::vector<std::string> args;

    // Parse arguments to strings
    args.reserve(argc);
    for (int i = 1; i < argc; ++i) {
        args.push_back(argv[i]);
    }

    // Parse strings
    for (std::size_t i = 0; i < args.size(); ++i) {
        if (args[i].empty() || args[i][0] != '-')
            continue; // skip invalid arguments

        std::string argName = args[i].substr(1);

        if ((i + 1) < args.size() && !args[i + 1].empty() && args[i + 1][0] != '-') {
            // this is `-arg value` pair
            arguments[argName] = args[i + 1];
            ++i;
        } else {
            // this is last argument or next argument starts with '-'
            arguments[argName] = "";
        }
    }
}

bool ArgumentParser::hasArgument(const std::string& argumentName) const
{
    return (arguments.find(argumentName) != arguments.end());
}

std::string ArgumentParser::getArgument(const std::string& argumentName) const
{
    return arguments.at(argumentName);
}

int ArgumentParser::getIntArgument(const std::string& argumentName) const
{
    return std::stoi(getArgument(argumentName));
}

float ArgumentParser::getFloatArgument(const std::string& argumentName) const
{
    return std::stof(getArgument(argumentName));
}

const std::string& ArgumentParser::getPath() const
{
    return path;
}
}
