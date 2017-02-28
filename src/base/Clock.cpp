#include <base/Clock.h>

#include <fstream>
#include <string>

namespace base {
Clock::Clock()
{
    reset();
}

void Clock::reset()
{
    TimePoint tpNow = now();

    _tpStart = _tpLast = tpNow;
}

Clock::TimePoint Clock::now()
{
    return _NativeClock::now();
}

Clock::Duration Clock::getElapsedTime()
{
    TimePoint tpNow = now();
    Duration result = tpNow - _tpLast;

    _tpLast = tpNow;

    return result;
}

Clock::Duration Clock::getTotalTime()
{
    TimePoint tpNow = now();
    Duration result = tpNow - _tpStart;

    _tpLast = tpNow;

    return result;
}
}
