#include <base/File.h>

#include <fstream>
#include <iostream>
#include <stdexcept>

namespace base {

bool File::exists(const std::string& path)
{
    bool result;
    std::ifstream file(path.c_str());

    result = file.good();

    return result;
}

std::string File::readText(const std::string& path, bool throwException)
{
    std::string result;
    std::ifstream fileStream(path, std::fstream::in);
    if (fileStream.is_open()) {
        std::string line = "";

        while (std::getline(fileStream, line)) {
            result += line;

            if (fileStream.eof() == false) {
                result += "\n";
            }
        }

        fileStream.close();
    } else {
        std::cerr << "base::File::readText > Couldn't open file: " << path << std::endl;

        if (throwException) {
            std::string errorMsg = "Could not open file: '" + path + "'";
            throw std::runtime_error(errorMsg);
        }
    }
    return result;
}

std::string File::readBinary(const std::string& path, bool throwException)
{
    std::string result;
    std::ifstream file(path, std::ios::in | std::ios::binary);
    if (file) {
        file.seekg(0, std::ios::end);
        result.resize(static_cast<std::size_t>(file.tellg()));
        file.seekg(0, std::ios::beg);
        file.read(&result.front(), result.size());
        file.close();
    } else {
        std::cerr << "base::File::readBinary > Couldn't open file: " << path << std::endl;

        if (throwException) {
            throw std::runtime_error("Couldn't open file: '" + path + "'");
        }
    }
    return result;
}

std::string File::getPath(const std::string& path)
{
    return path.substr(0, path.find_last_of("/\\") + 1);
}

std::string File::getFilename(const std::string& path)
{
    return path.substr(path.find_last_of("/\\") + 1);
}

std::string File::getFilenameExtensionless(const std::string& path)
{
    std::string fileName = File::getFilename(path);
    return fileName.substr(0, fileName.find_last_of("."));
}

std::string File::getExtension(const std::string& filePath)
{
    return filePath.substr(filePath.find_last_of(".") + 1);
}
}
