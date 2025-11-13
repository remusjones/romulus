#pragma once
#include <fstream>
#include <mutex>
#include <stack>

#include "Logger.h"
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
struct TimerResult;
struct ScopedProfileTimer;

class Profiler final : public ImGuiLayer{
public:
    // TODO: Maybe make a profiler factory for different sessions/groups?
    static Profiler& GetInstance() {
        static Profiler instance;
        return instance;
    }

    Profiler(Profiler const&) = delete;
    void operator=(Profiler const&) = delete;

    void EndSample(const TimerResult &aResult);

    void BeginProfile(const char *aName, const char *aFunctionSignature, const int aLineNumber);
    void EndProfile();


    Profiler();
    ~Profiler() override;
private:
    void EnsureProfilerLimits(const std::string &aName);
    void StartTraceSession();
    void ExportTraceFrame(const TimerResult& aResult);
    void EndTraceSession();

public:
    void OnImGuiRender() override;

private:

    bool mSerializeTrace = false;
    bool mRunning = true;
    std::stack<ManagedProfileTimer> mTimerStack;
    std::unordered_map<std::string, std::deque<TimerResult>> mTimerHistory;
    const int mMaxDisplayedHistorySize = 100;
    const char* mSessionTraceFilename = "trace.json";
    std::mutex mTraceWriteMutex;
    std::ofstream mSessionOutputStream;
    long long mTraceFrameCounter = 0;
};
