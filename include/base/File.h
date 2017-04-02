#pragma once

#include <string>

namespace base {
class File
{
  public:
    static bool exists(const std::string& path);

    static std::string readText(const std::string& path, bool throwException = false);

    static std::string getPath(const std::string& path);
    static std::string getFilename(const std::string& path);
    static std::string getFilenameExtensionless(const std::string& path);
    static std::string getExtension(const std::string& path);
};
}
