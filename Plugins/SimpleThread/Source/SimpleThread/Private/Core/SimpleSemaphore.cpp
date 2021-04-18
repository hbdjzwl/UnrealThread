#include "Core/SimpleSemaphore.h"

FSimpleSemaphore::FSimpleSemaphore()
	:Event(FPlatformProcess::GetSynchEventFromPool())	//拿到我们的Event
	,bWait(false)
{

}

FSimpleSemaphore::~FSimpleSemaphore()
{
	FPlatformProcess::ReturnSynchEventToPool(Event);	//释放事件对象
}

void FSimpleSemaphore::Wait()
{
	bWait = true;
	Event->Wait();
}

void FSimpleSemaphore::Wait(uint32 WaitTime, const bool bIgnoreThreadIdleStats /*= false*/)
{
	bWait = true;
	Event->Wait(WaitTime, bIgnoreThreadIdleStats);
}

void FSimpleSemaphore::Trigger()
{
	Event->Trigger();
	bWait = false;
}

