#include <string>
#include <chrono>

#pragma once

enum class TimeFormat { Milli, Micro, Seconds };
using ClockType = decltype(std::chrono::high_resolution_clock::now());

template <class Clock>
long long GetDuration(TimeFormat format, Clock start, Clock stop)
{
    switch (format) {
        case TimeFormat::Micro:
            return std::chrono::duration_cast<std::chrono::microseconds>(stop - start)
                .count();
        case TimeFormat::Milli:
            return std::chrono::duration_cast<std::chrono::milliseconds>(stop - start)
                .count();
        case TimeFormat::Seconds:
            return std::chrono::duration_cast<std::chrono::seconds>(stop - start).count();
        default:
            return std::chrono::duration_cast<std::chrono::microseconds>(stop - start)
                .count();
    }
}

inline std::string GetTimeFormatAbbreviation(TimeFormat format)
{
    switch (format) {
        case TimeFormat::Micro:
            return "us";
        case TimeFormat::Milli:
            return "ms";
        case TimeFormat::Seconds:
            return "s";
        default:
            return "us";
    }
}