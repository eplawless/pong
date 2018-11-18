#include "timer.h"

#include <Windows.h>

Timer::Timer()
{
	Start();
}

void Timer::Start()
{
	LARGE_INTEGER startTimeStruct;
	QueryPerformanceCounter(&startTimeStruct);
	m_startTime = startTimeStruct.QuadPart;
}

int64_t Timer::GetElapsedMicroseconds() const
{
	LARGE_INTEGER currentTimeStruct;
	QueryPerformanceCounter(&currentTimeStruct);
	return currentTimeStruct.QuadPart - m_startTime;
}
