#include <pch.h>
#include "biome_core/Time/Timer.h"

using namespace biome::time;

struct Timer::TimerImpl
{
	static LARGE_INTEGER GetFrequency()
	{
		LARGE_INTEGER frequency;
		BIOME_ASSERT_ALWAYS_EXEC(QueryPerformanceFrequency(&frequency));
		return frequency;
	}

	static LARGE_INTEGER GetCurrentTime()
	{
		LARGE_INTEGER currentTime;
		BIOME_ASSERT_ALWAYS_EXEC(QueryPerformanceCounter(&currentTime));
		return currentTime;
	}

	TimerImpl()
		: m_Frequency(GetFrequency())
		, m_StartTime(GetCurrentTime())
	{

	}

	~TimerImpl() = default;

	float GetElapsedSecondsSinceStart() const
	{
		const LARGE_INTEGER currentTime = GetCurrentTime();
		LARGE_INTEGER elapsed = {};
		elapsed.QuadPart = currentTime.QuadPart - m_StartTime.QuadPart;
		elapsed.QuadPart /= m_Frequency.QuadPart;

		return static_cast<float>(elapsed.QuadPart);
	}

	float GetElapsedSecondsSinceLastCall()
	{
		const LARGE_INTEGER currentTime = GetCurrentTime();
		const float elapsedSecs = (currentTime.QuadPart - m_LastTime.QuadPart) / (1.f * m_Frequency.QuadPart);
		m_LastTime = currentTime;

		return elapsedSecs;
	}

	void Reset()
	{
		m_StartTime = m_LastTime = GetCurrentTime();
	}

	const LARGE_INTEGER m_Frequency {};
	LARGE_INTEGER m_StartTime {};
	LARGE_INTEGER m_LastTime {};
};

Timer::Timer()
	: pImpl(new TimerImpl())
{
	
}

Timer::~Timer()
{

}

float Timer::GetElapsedSecondsSinceStart() const
{
	return pImpl->GetElapsedSecondsSinceStart();
}

float Timer::GetElapsedSecondsSinceLastCall() const
{
	return pImpl->GetElapsedSecondsSinceLastCall();
}

void Timer::Reset()
{
	pImpl->Reset();
}
