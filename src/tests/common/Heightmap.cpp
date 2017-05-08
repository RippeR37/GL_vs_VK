#include <tests/common/Heightmap.h>

#include <base/File.h>

#include <stdexcept>

namespace tests {
namespace common {
Heightmap::Heightmap(const std::string& path, std::size_t width, std::size_t height)
    : _size({width, height})
    , _data(base::File::readBinaryBytes(path, true))
{
}

const glm::uvec2& Heightmap::getSize() const
{
    return _size;
}

const std::vector<uint8_t> Heightmap::getData() const
{
    return _data;
}

uint8_t Heightmap::getHeight(std::size_t x, std::size_t y) const
{
    if (x >= getSize().x || y >= getSize().y) {
        throw std::invalid_argument{"Attempting to access out-of-range heightmap field"};
    }

    return _data.at(x * getSize().x + y);
}
}
}
