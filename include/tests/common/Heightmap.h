#pragma once

#include <glm/vec2.hpp>

#include <cstdint>
#include <string>
#include <vector>

namespace tests {
namespace common {
class Heightmap
{
  public:
    Heightmap(const std::string& path, std::size_t width, std::size_t height);
    Heightmap(const Heightmap&) = delete;

    Heightmap& operator=(const Heightmap&) = delete;

    const glm::uvec2& getSize() const;
    const std::vector<uint8_t> getData() const;
    uint8_t getHeight(std::size_t x, std::size_t y) const;

  private:
    glm::uvec2 _size;
    std::vector<uint8_t> _data;
};
}
}
