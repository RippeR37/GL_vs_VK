#pragma once

#ifdef ENABLE_TIMINGS
#define TIME_IT(x) auto __scopedTimer = ::base::ScopedTimer{x};
#define TIME_RESET(x) ::base::ScopedTimer::reset(x);
#else
#define TIME_IT(x)
#define TIME_RESET(x)
#endif

namespace base {
class ScopedTimer
{
  public:
    ScopedTimer(const char* message);
    ~ScopedTimer();

    static void reset(const char* message);

  private:
    const char* _message;
    double _start;
};
}
