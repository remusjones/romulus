#include "Profiler.h"

#include "imgui.h"
#include "Logger.h"
#include "ScopedProfileTimer.h"

#define TRACE 0 // Move to state or config

Profiler::Profiler()
{
#if TRACE
	StartTraceSession();
#endif
}

Profiler::~Profiler()
{
#if TRACE
	EndTraceSession();
#endif

	while (!timerStack.empty())
	{
		const auto item = timerStack.top();
		Logger::LogError(std::string("Profiler: Timer stack not empty at destruction ") + item.profilerName);
		timerStack.pop();
	}
}

void Profiler::EndSample(const TimerResult& result)
{
	timerHistory[result.timerName].emplace_back(result);

#if TRACE
	ExportTraceFrame(result);
#endif
	EnsureProfilerLimits(result.timerName);
}

void Profiler::BeginProfile(const char* profilerName, const char* functionSignature, const int lineNumber)
{
	if (!running)
	{
		return;
	}

	timerStack.emplace(profilerName, *this, functionSignature, lineNumber);
}

void Profiler::EndProfile()
{
	if (!running || timerStack.empty()) // Catch incase of mismatched begin/end
	{
		return;
	}

	// TODO: This doesn't allow us to profile different threads without race condition, and is not thread safe
	// Perhaps migrate to a ID system timer stack and use a map to store the results
	timerStack.top().StopTimer();
	timerStack.pop();
}

void Profiler::EnsureProfilerLimits(const std::string& aName)
{
	auto& history = timerHistory[aName];
	while (history.size() > maxDisplayedHistoryTime)
	{
		history.pop_front();
	}
}

// TODO: Migrate to protobuf format instead of JSON
void Profiler::StartTraceSession()
{
	if (sessionOutputStream.is_open())
	{
		EndTraceSession();
	}

	sessionOutputStream.open(sessionTraceFilename, std::ios::out);
	sessionOutputStream << "{\"otherData\": {}, \"traceEvents\":[";
	sessionOutputStream.flush();
}

// TODO: Convert to ProtoBuf format
void Profiler::ExportTraceFrame(const TimerResult& aResult)
{
	std::lock_guard lock(traceWriteMutex);

	if (traceFrameCounter++ > 0)
	{
		sessionOutputStream << ",";
	}

	std::string name = aResult.timerName;
	std::replace(name.begin(), name.end(), '"', '\'');

	if (!name.empty())
	{
		name.append("_");
	}
	name.append(std::string(aResult.timerMetadata.functionSignature) + ":" + std::to_string(aResult.timerMetadata.lineNumber));

	sessionOutputStream << "{";
	sessionOutputStream << "\"name\":\"" << name << "\",";
	sessionOutputStream << "\"ph\":\"X\",";
	sessionOutputStream << "\"ts\":" << aResult.timerStart << ",";
	sessionOutputStream << "\"dur\":" << aResult.timerEnd - aResult.timerStart << ",";
	sessionOutputStream << "\"pid\":0,";
	sessionOutputStream << "\"tid\":" << aResult.threadId;
	sessionOutputStream << "}";
	sessionOutputStream.flush();
	traceWriteMutex.unlock();
}

void Profiler::EndTraceSession()
{
	if (!sessionOutputStream.is_open())
	{
		return;
	}

	sessionOutputStream << "]}";
	sessionOutputStream.flush();
	sessionOutputStream.close();
}

void Profiler::OnImGuiRender()
{
	ImGui::Begin("Profiler");
	if (ImGui::Checkbox("Running", &running))
	{
		if (!running)
		{
			while (!timerStack.empty())
			{
				timerStack.top().StopTimer();
				timerStack.pop();
			}
			EndTraceSession();
		}
		else
		{
			if (serializeTrace)
			{
				StartTraceSession();
			}
		}
	}
	if (ImGui::Checkbox("Serialize Trace", &serializeTrace))
	{
		if (serializeTrace)
		{
			StartTraceSession();
		}
		else
		{
			EndTraceSession();
		}
	}

	for (auto& [fst, snd] : timerHistory)
	{
		if (snd.empty())
		{
			continue;
		}

		std::deque<TimerResult>& tempDeque = snd;
		std::vector<float> durations;

		float sum = 0.0f;
		const int count = tempDeque.size();

		for (auto& info : tempDeque)
		{
			float duration = info.GetDurationMilliseconds().count();
			durations.push_back(duration);
			sum += duration;
		}

		const float average = count > 0 ? sum / static_cast<float>(count) : 0.0f;
		char overlay[32];
		sprintf(overlay, "%.3f ms", average);
		if (!durations.empty())
		{
			ImGui::PlotHistogram(fst.c_str(), &durations[0], count, 0, overlay, 0.0f, 2.0f);
		}
	}
	ImGui::End();
}
