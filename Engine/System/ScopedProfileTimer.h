#pragma once
#include <chrono>
#include <thread>

#include "EASTL/string_view.h"

class Profiler;

struct TimerMetadata
{
	TimerMetadata(const eastl::string_view& inFunctionSignature, int inLineNumber);

	TimerMetadata();

	eastl::string_view functionSignature;
	const int lineNumber;
};

struct TimerResult
{
	eastl::string_view timerName{};
	long long timerStart{}, timerEnd{};
	int threadId{};

	TimerMetadata timerMetadata;

	[[nodiscard]] std::chrono::duration<float> GetDurationMilliseconds() const;

	[[nodiscard]] std::chrono::microseconds GetDurationMicroseconds() const;
};

// Todo: create base class for scoped and managed timer?
struct ScopedProfileTimer
{
	eastl::string_view profileName;
	Profiler* profiler;
	std::chrono::time_point<std::chrono::system_clock> start, end;
	TimerMetadata metadata;

	explicit ScopedProfileTimer(const eastl::string_view& aName, Profiler& aBoundProfiler, const eastl::string_view& aFunctionSignature,
	                            int aLineNumber);

	~ScopedProfileTimer();

	[[nodiscard]] TimerResult GetResult() const;
};

struct ManagedProfileTimer
{
	eastl::string_view profilerName;
	Profiler* profiler;
	std::chrono::time_point<std::chrono::system_clock> start, end;
	TimerMetadata metadata;

	explicit ManagedProfileTimer(const eastl::string_view& inProfilerName, Profiler& inProfiler, const eastl::string_view& functionSignature,
	                             int lineNumber);

	void StopTimer();

	[[nodiscard]] TimerResult GetResult() const;
};
