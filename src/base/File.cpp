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
    std::ifstream fileStream;
    std::string result = "";
    std::string line = "";

    fileStream.open(path, std::fstream::in);
    if (fileStream.is_open()) {
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
