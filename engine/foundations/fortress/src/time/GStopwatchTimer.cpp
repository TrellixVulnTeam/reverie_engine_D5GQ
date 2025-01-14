#include "fortress/time/GStopwatchTimer.h"
#include <sstream>

namespace rev {

StopwatchTimer StopwatchTimer::FromElapsedSeconds(float seconds)
{
    StopwatchTimer timer;
    auto duration = std::chrono::duration_cast<std::chrono::steady_clock::duration>(
        std::chrono::duration<float, std::ratio<1, 1>>(seconds));
    timer.m_startTime = clock::now();
    timer.m_startTime = timer.m_startTime - duration;
    return timer;
}

std::time_t StopwatchTimer::SteadyToTime_t(const clock::time_point & time)
{
    return std::chrono::system_clock::to_time_t(std::chrono::system_clock::now() + 
        std::chrono::duration_cast<std::chrono::system_clock::duration>(time -
        std::chrono::steady_clock::now()));
}

GString StopwatchTimer::ToString(const clock::time_point & time, const GString& formatIn)
{
    static GString defaultFormat = "UTC: %Y-%m-%d %H:%M:%S";
    const GString* format;
    if (formatIn.isNull()) {
        format = &defaultFormat;
    }
    else {
        format = &formatIn;
    }

    std::time_t tt = SteadyToTime_t(time);
    std::tm tm = *std::gmtime(&tt); //GMT (UTC)
    //std::tm tm = *std::localtime(&tt); //Locale time-zone, usually UTC by default.
    std::stringstream ss;
    ss << std::put_time(&tm, format->c_str());

    return ss.str().c_str();
}

StopwatchTimer::StopwatchTimer()
{
}

StopwatchTimer::StopwatchTimer(const StopwatchTimer & other):
    m_startTime(other.m_startTime),
    m_elapsedTime(other.m_elapsedTime)
{
}

StopwatchTimer::~StopwatchTimer()
{
}

StopwatchTimer & StopwatchTimer::operator=(const StopwatchTimer & other)
{
    m_startTime = other.m_startTime;
    m_elapsedTime = other.m_elapsedTime;
    return *this;
}


// End namespaces        
}