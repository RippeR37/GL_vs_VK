#include <base/ScopedTimer.h>

#include <GLFW/glfw3.h>

#include <cstdio>

namespace base {
ScopedTimer::ScopedTimer(const char* message)
    : _message(message)
    , _start(glfwGetTime())
{
}

ScopedTimer::~ScopedTimer()
{
    fprintf(stderr, "-- %s (%.3f ms)\n", _message, (glfwGetTime() - _start) * 1000.0);
}

void ScopedTimer::reset(const char* message)
{
    fprintf(stderr, "\n%s\n", message);
}
}
