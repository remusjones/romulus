#pragma once
#include <fstream>
#include <mutex>


#include "ScopedProfileTimer.h"
#include "EASTL/deque.h"
#include "EASTL/stack.h"
#include "Objects/ImGuiLayer.h"

#ifdef _MSC_VER
#define FUNCTION_SIGNATURE __FUNCSIG__
#else
#define FUNCTION_SIGNATURE __PRETTY_FUNCTION__
#endif

#define PROFILE_SCOPE(name) ScopedProfileTimer timer##__LINE__ = (Profiler::GetInstance().mRunning ? ScopedProfileTimer(name, Profiler::GetInstance(), FUNCTION_SIGNATURE, __LINE__) : ScopedProfileTimer())
#define PROFILE_FUNCTION_SCOPED() PROFILE_SCOPE(FUNCTION_SIGNATURE)
#define PROFILE_FUNCTION_SCOPED_NAMED(name) PROFILE_SCOPE(name)

#define PROFILE_BEGIN(name) Profiler::GetInstance().BeginProfile(name, FUNCTION_SIGNATURE, __LINE__)
#define PROFILE_END() Profiler::GetInstance().EndProfile()

struct ManagedProfileTimer;
struct ScopedProfileTimer;

class Profiler final : public ImGuiLayer
{
public:
	// TODO: Maybe make a profiler factory for different sessions/groups?
	static Profiler& GetInstance()
	{
		static Profiler instance;
		return instance;
	}

	Profiler(Profiler const&) = delete;
	void operator=(Profiler const&) = delete;

	void EndSample(const TimerResult& result);

	void BeginProfile(const char* profilerName, const char* functionSignature, int lineNumber);
	void EndProfile();


	Profiler();
	~Profiler() override;

private:
	void EnsureProfilerLimits(const eastl::string_view& aName);
	void StartTraceSession();
	void ExportTraceFrame(const TimerResult& aResult);
	void EndTraceSession();

public:
	void OnImGuiRender() override;

private:
	bool serializeTrace = false;
	bool running = true;
	eastl::stack<ManagedProfileTimer> timerStack;
	eastl::hash_map<eastl::string_view, eastl::deque<TimerResult>> timerHistory;
	const int maxDisplayedHistoryTime = 100;
	const char* sessionTraceFilename = "trace.json";
	std::mutex traceWriteMutex;
	std::ofstream sessionOutputStream;
	long long traceFrameCounter = 0;
};
