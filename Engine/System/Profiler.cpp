#include "Profiler.h"

#include "imgui.h"
#include "ScopedProfileTimer.h"
#include "spdlog/spdlog.h"

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
		SPDLOG_ERROR("Profiler: Timer Stack no empty at destruction {}", item.profilerName);
		timerStack.pop();
	}
}

void Profiler::EndSample(const TimerResult& result)
{
	if (timerHistory.contains(result.timerName))
	{
		timerHistory.at(result.timerName).emplace_back(result);
	}else
	{
		eastl::deque<TimerResult> newDeque ({result});
		timerHistory.emplace(result.timerName, newDeque);
	}

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

void Profiler::EnsureProfilerLimits(const eastl::string_view& aName)
{
	auto& history = timerHistory.at(aName);
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

	eastl::string nameBuilder = aResult.timerName.data();
	//std::replace(nameBuilder.begin(), nameBuilder.end(), '"', '\'');
//
	//if (!nameBuilder.empty())
	//{
	//	nameBuilder.append("_");
	//}
//
	//nameBuilder.append(aResult.timerMetadata.functionSignature);
	//nameBuilder.append(":");
	//nameBuilder.append(eastl::to_string(aResult.timerMetadata.lineNumber));

	auto duration = aResult.timerEnd - aResult.timerStart;

	std::format_to(
		std::ostreambuf_iterator<char>(sessionOutputStream),
		"{{\"name\":\"{}\",\"ph\":\"X\",\"ts\":{},\"dur\":{},\"pid\":0,\"tid\":{}}}",
		std::string_view(nameBuilder.data(), nameBuilder.length()),
		aResult.timerStart,
		duration,
		aResult.threadId
	);

	sessionOutputStream.flush();

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

		eastl::deque<TimerResult>& tempDeque = snd;
		eastl::vector<float> durations;

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
			ImGui::PlotHistogram(fst.data(), &durations[0], count, 0, overlay, 0.0f, 2.0f);
		}
	}
	ImGui::End();
}
