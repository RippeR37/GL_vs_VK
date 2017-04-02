#pragma once

#include <string>
#include <vector>

namespace base {
class String
{
  public:
    typedef std::vector<std::string> stringVector;

  public:
    static stringVector& split(stringVector& input, const std::string& data, char delimiter, bool skipEmpty = true);
    static stringVector split(const std::string& data, char delimiter, bool skipEmpty = true);

    static std::string& trim(std::string& string);
    static std::string& ltrim(std::string& string);
    static std::string& rtrim(std::string& string);

    static bool startsWith(const std::string& string, const std::string& prefix);
    static bool endsWith(const std::string& string, const std::string& sufix);
};
}
