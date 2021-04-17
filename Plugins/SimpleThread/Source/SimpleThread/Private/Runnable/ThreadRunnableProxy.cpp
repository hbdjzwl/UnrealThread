#include "Runnable/ThreadRunnableProxy.h"
#include "HAL/RunnableThread.h"

int32 FThreadRunnable::ThreadCount = 0;

FThreadRunnable::FThreadRunnable()
	:IThreadProxy()
	,bRun(false)
	,bSuspend(true) //默认创建后是直接挂起线程的
	,Thread(nullptr)
	,ThreadEvent(FPlatformProcess::GetSynchEventFromPool()) //拿到我们的Event
	,StartUpEvent(FPlatformProcess::GetSynchEventFromPool())
	,WaitExecuteEvent(FPlatformProcess::GetSynchEventFromPool())
{

}

FThreadRunnable::FThreadRunnable(bool InSuspend)
	:IThreadProxy()
	, bRun(false)
	, bSuspend(InSuspend)
	, Thread(nullptr)
	, ThreadEvent(FPlatformProcess::GetSynchEventFromPool()) //拿到我们的Event
	, StartUpEvent(FPlatformProcess::GetSynchEventFromPool())
	, WaitExecuteEvent(FPlatformProcess::GetSynchEventFromPool())
{

}

FThreadRunnable::~FThreadRunnable()
{
	//释放事件对象
	FPlatformProcess::ReturnSynchEventToPool(ThreadEvent);
	FPlatformProcess::ReturnSynchEventToPool(StartUpEvent);
	FPlatformProcess::ReturnSynchEventToPool(WaitExecuteEvent);

	if (Thread != NULL)
	{
		delete Thread;
		Thread = nullptr;//意义不大
	}
}
void FThreadRunnable::CreateSafeThread()
{
	RunnableName = *FString::Printf(TEXT("SimpleThread-%i"), ThreadCount);
	Thread = FRunnableThread::Create(this, *RunnableName.ToString(), 0, TPri_BelowNormal);

	ThreadCount++;
}

void FThreadRunnable::WakeupThread() //主线程调用执行
{
	ThreadEvent->Trigger();//唤醒沉睡的线程
}

bool FThreadRunnable::IsSuspend()
{
	return bSuspend;
}

void FThreadRunnable::WaitAndCompleted()
{
	bRun = false;
	ThreadEvent->Trigger(); //激活原有线程？

	StartUpEvent->Wait(); //阻塞我门的启动线程 (此行等待唤醒)

	//主线程休眠0.03秒。等待当前线程Run()成功返回后。在继续执行主线程后续的删除线程任务。
	FPlatformProcess::Sleep(0.03f); 
}

void FThreadRunnable::BlockingAndCompletion()
{
	WaitExecuteEvent->Wait();
}

uint32 FThreadRunnable::Run()
{
	while(bRun)
	{
		if (bSuspend)
		{
			ThreadEvent->Wait(); //挂起线程
		}

		bSuspend = false;

		/* 业务逻辑 */
		if (ThreadDelegate.IsBound()) //代理是否绑定
		{
			ThreadDelegate.Execute(); //ProxyInterface::ThreadDelegate
			ThreadDelegate.Unbind(); //解除代理绑定
		}

		//执行完任务挂起这个线程
		bSuspend = true;

		//激活挂起的启动线程
		WaitExecuteEvent->Trigger();

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
	StartUpEvent->Trigger(); //唤醒主线程
	bRun = false;
}