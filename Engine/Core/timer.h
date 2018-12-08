#pragma once
#include <cstdint>

class Timer
{
public: // methods
	Timer();
	void Start();
	int64_t GetElapsedMicroseconds() const;

private: // members
	int64_t m_startTime;
};