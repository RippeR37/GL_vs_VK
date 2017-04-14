#include <base/ContainerUtils.h>

namespace base {
std::vector<const char*> viewOf(const std::vector<std::string>& container)
{
    std::vector<const char*> view(container.size(), nullptr);

    for (std::size_t index = 0; index < container.size(); ++index)
        view[index] = container[index].c_str();

    return view;
}
}
