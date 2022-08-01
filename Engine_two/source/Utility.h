#pragma once
#include <windows.h>
#include <Windowsx.h>
class Utility
{
};





class Timer
{
public:
	Timer();
	float DeltaTime() const;
	float TotalTime() const;
	void Reset();
	void Start();
	void Stop();
	void Tick();
private:
	double mSecondsPerCount{};
	double mDeltaTime{ 0 };

	__int64 mBaseTime{ 0 };
	__int64 mPausedTime{ 0 };
	__int64 mStopTime{ 0 };
	__int64 mPrevTime{ 0 };
	__int64 mCurrTime{ 0 };

	bool mStopped {};
};
