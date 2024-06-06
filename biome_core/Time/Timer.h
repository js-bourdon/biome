#pragma once

#include <memory>

namespace biome::time
{
	class Timer
	{
	public:

		Timer();
		~Timer();

		float GetElapsedSecondsSinceStart() const;
		float GetElapsedSecondsSinceLastCall() const;
		void Reset();

	private:

		struct TimerImpl;
		std::unique_ptr<TimerImpl> pImpl {};
	};
}