#include "Runnable/ThreadRunnableProxy.h"
#include "HAL/RunnableThread.h"

int32 FThreadRunnable::ThreadCount = 0;

FThreadRunnable::FThreadRunnable()
	:IThreadProxy()
	,bRun(false)
	,bSuspend(false)
	,bImplement(false)
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
	//准备执行逻辑
	bSuspend = true;

	RunnableName = *FString::Printf(TEXT("SimpleThread-%i"), ThreadCount);
	Thread = FRunnableThread::Create(this, *RunnableName.ToString(), 0, TPri_BelowNormal);

	ThreadCount++;
}

void FThreadRunnable::SuspendThread()
{
	bSuspend = true;

}

void FThreadRunnable::WakeupThread()
{
	bImplement = true;
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

		if (bImplement)
		{
			bImplement = false;

			/* 业务逻辑 */
			if (ThreadDelegate.IsBound()) //代理是否绑定
			{
				ThreadDelegate.Execute(); //ProxyInterface::ThreadDelegate
				ThreadDelegate.Unbind(); //解除代理绑定
			}
			else
			{
				ThreadLambda(); //ProxyInterface::ThreadLambda
				ThreadLambda = nullptr; //置空Lambda
			}

			//挂起这个线程
			SuspendThread();
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