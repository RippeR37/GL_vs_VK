#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace base {
class File
{
  public:
    static bool exists(const std::string& path);

    static std::string readText(const std::string& path, bool throwException = false);
    static std::string readBinary(const std::string& path, bool throwException = false);
    static std::vector<uint8_t> readBinaryBytes(const std::string& path, bool throwException = false);

    static std::string getPath(const std::string& path);
    static std::string getFilename(const std::string& path);
    static std::string getFilenameExtensionless(const std::string& path);
    static std::string getExtension(const std::string& path);
};
}
