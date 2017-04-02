#pragma once

#include <string>
#include <unordered_map>

namespace base {
class ArgumentParser
{
  public:
    ArgumentParser(int argc, char* argv[]);

    bool hasArgument(const std::string& argumentName) const;
    std::string getArgument(const std::string& argumentName) const;
    int getIntArgument(const std::string& argumentName) const;

    const std::string& getPath() const;

  private:
    std::string path;
    std::unordered_map<std::string, std::string> arguments;
};
}
