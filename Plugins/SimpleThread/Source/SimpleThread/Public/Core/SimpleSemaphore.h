#pragma once
#include "CoreMinimal.h"

//简单信号量
struct FSimpleSemaphore
{
	FSimpleSemaphore();
	~FSimpleSemaphore();
	void Wait();
	void Wait(uint32 WaitTime,const bool bIgnoreThreadIdleStats = false);
	void Trigger();
	FORCEINLINE bool IsWait() { return bWait; }
private:
	FEvent* Event; //信号量本量
	uint8 bWait : 1; //单字节位域，用于布尔
};