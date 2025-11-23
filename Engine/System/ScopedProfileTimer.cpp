//
// Created by remus on 16/11/2025.
//
#include <ScopedProfileTimer.h>
#include <Profiler.h>

TimerMetadata::TimerMetadata(const eastl::string_view& inFunctionSignature, const int inLineNumber): functionSignature(inFunctionSignature),
    lineNumber(inLineNumber)
{
}

std::chrono::duration<float> TimerResult::GetDurationMilliseconds() const
{
    return std::chrono::milliseconds(timerEnd - timerStart);
}

std::chrono::microseconds TimerResult::GetDurationMicroseconds() const
{
    return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::microseconds(timerEnd - timerStart));
}

ScopedProfileTimer::ScopedProfileTimer(const eastl::string_view& aName, Profiler& aBoundProfiler, const eastl::string_view& aFunctionSignature,
    const int aLineNumber): metadata(aFunctionSignature, aLineNumber)
{
    start = std::chrono::steady_clock::now();
    profileName = aName;
    profiler = &aBoundProfiler;
}

ScopedProfileTimer::~ScopedProfileTimer()
{
    end = std::chrono::steady_clock::now();
    profiler->EndSample(GetResult());
}

TimerResult ScopedProfileTimer::GetResult() const
{
    const int hash = static_cast<int>(std::hash<std::thread::id>{}(std::this_thread::get_id()));
    return {
        profileName,
        std::chrono::time_point_cast<std::chrono::microseconds>(start).time_since_epoch().count(),
        std::chrono::time_point_cast<std::chrono::microseconds>(end).time_since_epoch().count(),
        hash,
        metadata
    };
}

ManagedProfileTimer::ManagedProfileTimer(const eastl::string_view& inProfilerName, Profiler& inProfiler,
    const eastl::string_view& functionSignature, int lineNumber): metadata(functionSignature, lineNumber)
{
    start = std::chrono::steady_clock::now();
    profilerName = inProfilerName;
    profiler = &inProfiler;
}

void ManagedProfileTimer::StopTimer()
{
    end = std::chrono::steady_clock::now();
    profiler->EndSample(GetResult());
}

TimerResult ManagedProfileTimer::GetResult() const
{
    const int hash = static_cast<int>(std::hash<std::thread::id>{}(std::this_thread::get_id()));
    return {
        profilerName,
        std::chrono::time_point_cast<std::chrono::microseconds>(start).time_since_epoch().count(),
        std::chrono::time_point_cast<std::chrono::microseconds>(end).time_since_epoch().count(),
        hash,
        metadata
    };
}
