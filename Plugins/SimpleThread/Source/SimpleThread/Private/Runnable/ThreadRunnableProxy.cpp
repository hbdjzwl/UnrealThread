#include "Runnable/ThreadRunnableProxy.h"
#include "HAL/RunnableThread.h"
#include "SimpleThreadPlatform.h"  //自定义跨平台

int32 FThreadRunnable::ThreadCount = 0;

FThreadRunnable::FThreadRunnable(bool SuspendAtFirst)
	:IThreadProxy()
	,StopTaskCounter(0)
	,bSuspendAtFirst(SuspendAtFirst) //默认创建后是直接挂起线程的
	,Thread(nullptr)
{

}

FThreadRunnable::~FThreadRunnable()
{
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
	ThreadEvent.Trigger();//唤醒沉睡的线程
}

bool FThreadRunnable::IsSuspend()
{
	return ThreadEvent.IsWait();
}

void FThreadRunnable::WaitAndCompleted()
{
	Stop();
	if (ThreadEvent.IsWait()) //防止主线程被锁死(挂起)！
	{
		ThreadEvent.Trigger(); //激活原有线程？
		StartUpEvent.Wait(); //阻塞我门的启动线程 (此行等待唤醒)
		//主线程休眠0.03秒。等待当前线程Run()成功返回后。在继续执行主线程后续的删除线程任务。
		FPlatformProcess::Sleep(0.03f);
	}
}

void FThreadRunnable::BlockingAndCompletion()
{


	ThreadEvent.Trigger();//唤醒沉睡的线程
	WaitExecuteEvent.Wait();
}

uint32 FThreadRunnable::Run()
{
	while(StopTaskCounter.GetValue() == 0)
	{
		if (!bSuspendAtFirst) //第一次是否被挂起
		{
			ThreadEvent.Wait(); //挂起线程
		}

		/* 业务逻辑 */
		if (ThreadDelegate.IsBound()) //代理是否绑定
		{
			ThreadDelegate.Execute(); //ProxyInterface::ThreadDelegate
			ThreadDelegate.Unbind(); //解除代理绑定
		}

		//激活挂起的启动线程
		WaitExecuteEvent.Trigger();

		bSuspendAtFirst = false; //一次性变量用于判断第一次是否被挂起
	}

	return 0;
}

bool FThreadRunnable::Init()
{
	if (!FPlatformProcess::SupportsMultithreading()) //当前平台是否支持多线程
	{
		Stop();
	}

	return StopTaskCounter.GetValue() == 0;
}
void FThreadRunnable::Stop()
{
	StopTaskCounter.Increment(); //递增
}

void FThreadRunnable::Exit()
{
	StartUpEvent.Trigger(); //唤醒主线程
}