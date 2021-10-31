#include "DXTimer.h"

DXTimer::DXTimer()
{
	LastPoint = std::chrono::steady_clock::now();
}

float DXTimer::Mark()
{
	const auto Old = LastPoint;
	LastPoint = std::chrono::steady_clock::now();
	const std::chrono::duration<float> FrameTime = (LastPoint - Old);
	return FrameTime.count();
}

float DXTimer::Peek() const
{
	return std::chrono::duration<float>(std::chrono::steady_clock::now() - LastPoint).count();
}
