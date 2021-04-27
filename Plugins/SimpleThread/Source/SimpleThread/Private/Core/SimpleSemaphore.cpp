#include "Core/SimpleSemaphore.h"
//#include "HAL/Event.h" //û��Ҳ������
#include "SimpleThreadPlatform.h"  //�Զ����ƽ̨

FSimpleSemaphore::FSimpleSemaphore()
	:Event(FPlatformProcess::GetSynchEventFromPool())	//�õ����ǵ�Event
	,bWait(false)
{

}

FSimpleSemaphore::~FSimpleSemaphore()
{
	FPlatformProcess::ReturnSynchEventToPool(Event);	//�ͷ��¼�����
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

