#pragma once
#include <chrono>

class DXTimer
{
public:
	DXTimer();
	float Mark();
	float Peek() const;

private:
	std::chrono::steady_clock::time_point LastPoint;
};

