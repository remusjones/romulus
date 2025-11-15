#pragma once
#include <chrono>
#include <thread>

#include "Profiler.h"

struct TimerMetadata
{
	TimerMetadata(const char* inFunctionSignature, const int inLineNumber) : functionSignature(inFunctionSignature),
	                                                                       lineNumber(inLineNumber)
	{
	}

	TimerMetadata();

	const char* functionSignature;
	const int lineNumber;
};

struct TimerResult
{
	const char* timerName{};
	long long timerStart{}, timerEnd{};
	int threadId{};

	TimerMetadata timerMetadata;

	[[nodiscard]] std::chrono::duration<float> GetDurationMilliseconds() const
	{
		return std::chrono::milliseconds(timerEnd - timerStart);
	}

	[[nodiscard]] std::chrono::microseconds GetDurationMicroseconds() const
	{
		return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::microseconds(timerEnd - timerStart));
	}
};

// Todo: create base class for scoped and managed timer?
struct ScopedProfileTimer
{
	const char* profileName;
	Profiler* profiler;
	std::chrono::time_point<std::chrono::system_clock> start, end;
	TimerMetadata metadata;

	explicit ScopedProfileTimer(const char* aName, Profiler& aBoundProfiler, const char* aFunctionSignature,
	                            const int aLineNumber) : metadata(aFunctionSignature, aLineNumber)
	{
		start = std::chrono::high_resolution_clock::now();
		profileName = aName;
		profiler = &aBoundProfiler;
	}

	~ScopedProfileTimer()
	{
		end = std::chrono::high_resolution_clock::now();
		profiler->EndSample(GetResult());
	}

	[[nodiscard]] TimerResult GetResult() const
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
};

struct ManagedProfileTimer
{
	const char* profilerName;
	Profiler* profiler;
	std::chrono::time_point<std::chrono::system_clock> start, end;
	TimerMetadata metadata;

	explicit ManagedProfileTimer(const char* inProfilerName, Profiler& inProfiler, const char* functionSignature,
	                             const int lineNumber) : metadata(functionSignature, lineNumber)
	{
		start = std::chrono::high_resolution_clock::now();
		profilerName = inProfilerName;
		profiler = &inProfiler;
	}

	void StopTimer()
	{
		end = std::chrono::high_resolution_clock::now();
		profiler->EndSample(GetResult());
	}

	[[nodiscard]] TimerResult GetResult() const
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
};
