#include "Runnable/ThreadRunnableProxy.h"
#include "HAL/RunnableThread.h"

int32 FThreadRunnable::ThreadCount = 0;

FThreadRunnable::FThreadRunnable()
	:IThreadProxy()
	,bRun(false)
	,Thread(nullptr)
	,ThreadEvent(FPlatformProcess::GetSynchEventFromPool()) //拿到我们的Event
{

}

FThreadRunnable::~FThreadRunnable()
{
	//释放事件对象
	FPlatformProcess::ReturnSynchEventToPool(ThreadEvent);
	if (Thread != NULL)
	{

	}
}
void FThreadRunnable::CreateSafeThread()
{
	RunnableName = *(TEXT("SimpleThread-") + FString::Printf(TEXT("%i"), ThreadCount));
	Thread = FRunnableThread::Create(this, *RunnableName.ToString(), 0, TPri_BelowNormal);
	ThreadCount++;
}

void FThreadRunnable::SuspendThread()
{
	bSuspend = true;

}

void FThreadRunnable::WakeupThread()
{
	bSuspend = false;
	ThreadEvent->Trigger();//唤醒沉睡的线程
}

uint32 FThreadRunnable::Run()
{
	while(bRun)
	{
		if (bSuspend)
		{
			ThreadEvent->Wait(); //挂起线程
		}

		/* 业务逻辑 */
		if (ThreadDelegate.IsBound()) //代理是否绑定
		{
			ThreadDelegate.Execute(); //ProxyInterface::ThreadDelegate
		}
		else
		{
			ThreadLambda(); //ProxyInterface::ThreadLambda
		}

	}

	return 0;
}

bool FThreadRunnable::Init()
{
	if (!FPlatformProcess::SupportsMultithreading()) //当前平台是否支持多线程
	{
		bRun = false;

	}
	else
	{
		bRun = true;
	}

	return bRun;
}
void FThreadRunnable::Stop()
{

}

void FThreadRunnable::Exit()
{
	bRun = false;
}