/*
 * Timer.cpp
 *
 * Li-Yi Wei
 * 4/15/2003
 *
 */

#include "Timer.hpp"

Timer::Timer(void) : _started(0)
{
    // nothing to do
}

Timer::~Timer(void)
{
    // nothing to do
}

int Timer::Start(void)
{
#ifdef WIN32
    if(timeBeginPeriod(1) == TIMERR_NOERROR)
    {
        _started = 1;
        _startRealTime = timeGetTime();
        return 1;
    }
    else
    {
        return 0;
    }
#else
    _started = 1;
    gettimeofday(&_startRealTime, 0);

    return 1;
#endif
}

int Timer::Stop(void)
{
#ifdef WIN32
    _started = 0;
    _stopRealTime = timeGetTime();
    timeEndPeriod(1);
#else
    gettimeofday(&_stopRealTime, 0);
#endif
    return 1;
}

double Timer::ElapsedTime(const Time & startTime,
              const Time & endTime) const
{
#ifdef WIN32
    return (_stopRealTime - _startRealTime)*0.001;
#else
    return ( _stopRealTime.tv_sec +  _stopRealTime.tv_usec / 1000000.0)
         - (_startRealTime.tv_sec + _startRealTime.tv_usec / 1000000.0);
#endif
}

double Timer::ElapsedTime(void) const
{
    return ElapsedTime(_startRealTime, _stopRealTime);
}

double Timer::CurrentTime(void) const
{
    if(_started)
    {
        return 0;
    }
#ifdef WIN32
    if(timeBeginPeriod(1) == TIMERR_NOERROR)
    {
        Time value = timeGetTime();
        timeEndPeriod(1);
        return value * 0.001;
    }
    else
    {
        return 0;
    }
#else
    Time value;
    gettimeofday(&value, 0);
    return ( value.tv_sec +  value.tv_usec / 1000000.0);
#endif
}
