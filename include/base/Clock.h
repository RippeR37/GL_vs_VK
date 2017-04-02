#pragma once

#include <chrono>

namespace base {
class Clock
{
  private:
    using _NativeClock = std::chrono::high_resolution_clock;
    using _NativeDuration = _NativeClock::duration;
    using _NativeTimePoint = _NativeClock::time_point;

  public:
    class Duration : _NativeDuration
    {
      public:
        Duration() : _NativeDuration() {}

        Duration(const _NativeDuration& duration) : _NativeDuration(duration) {}

        inline operator double() const { return asSeconds<double>(); }
        inline operator long long() const { return asSeconds<long long>(); }

        template <typename T>
        T asNanoseconds() const
        {
            return std::chrono::duration_cast<std::chrono::duration<T, std::nano>>(*this).count();
        }

        template <typename T>
        T asMicroseconds() const
        {
            return std::chrono::duration_cast<std::chrono::duration<T, std::micro>>(*this).count();
        }

        template <typename T>
        T asMilliseconds() const
        {
            return std::chrono::duration_cast<std::chrono::duration<T, std::milli>>(*this).count();
        }

        template <typename T>
        T asSeconds() const
        {
            return std::chrono::duration_cast<std::chrono::duration<T, std::ratio<1>>>(*this).count();
        }

        template <typename T>
        T asMinutes() const
        {
            return std::chrono::duration_cast<std::chrono::duration<T, std::ratio<60>>>(*this).count();
        }

        template <typename T>
        T asHours() const
        {
            return std::chrono::duration_cast<std::chrono::duration<T, std::ratio<3600>>>(*this).count();
        }
    };

    class TimePoint : _NativeTimePoint
    {
      public:
        TimePoint() : _NativeTimePoint() {}

        TimePoint(const _NativeTimePoint& tp) : _NativeTimePoint(tp) {}

        Duration operator-(const TimePoint& tp_other) const
        {
            return (static_cast<_NativeTimePoint>(*this) - tp_other);
        }

        bool operator==(const TimePoint& tp_other) const { return (static_cast<_NativeTimePoint>(*this) == tp_other); }

        bool operator!=(const TimePoint& tp_other) const { return (static_cast<_NativeTimePoint>(*this) != tp_other); }

        bool operator<(const TimePoint& tp_other) const { return (static_cast<_NativeTimePoint>(*this) < tp_other); }

        bool operator<=(const TimePoint& tp_other) const { return (static_cast<_NativeTimePoint>(*this) <= tp_other); }

        bool operator>(const TimePoint& tp_other) const { return (static_cast<_NativeTimePoint>(*this) > tp_other); }

        bool operator>=(const TimePoint& tp_other) const { return (static_cast<_NativeTimePoint>(*this) >= tp_other); }
    };

  public:
    Clock();

    void reset();

    Duration getElapsedTime();
    Duration getTotalTime();

    static TimePoint now();

  private:
    TimePoint _tpLast;
    TimePoint _tpStart;
};
}
