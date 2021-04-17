
#pragma once

#include "CoreMinimal.h"
#include "HAL/Runnable.h"
#include "Interface/ProxyInterface.h"

class FThreadRunnable : public FRunnable,public IThreadProxy
{
public:
	FThreadRunnable();
	FThreadRunnable(bool InSuspend);
	virtual ~FThreadRunnable();

	/* IThreadProxy的接口 */
	virtual void CreateSafeThread()override; //创建线程
	virtual void WakeupThread()override; //创建线程
	virtual bool IsSuspend()override; //线程是否挂起
	virtual void WaitAndCompleted()override; //启动我门当前线程的线程(可能是主线程)
	virtual void BlockingAndCompletion()override;	//阻塞并完成
private:
	/* Runnable的接口 */
	virtual uint32 Run()override;
	virtual bool Init()override;
	virtual void Stop()override;
	virtual void Exit()override;


private:
	bool				bRun; //判断线程时候在使用
	bool				bSuspend; //是否挂起线程
	FRunnableThread*	Thread; //线程实例
	FName				RunnableName; //线程名字
	FEvent*				ThreadEvent; //线程事件信号，挂起当前线程。谁执行挂起谁。(其他线程唤醒当前线程)
	FEvent*				StartUpEvent; //挂起主线程的信号
	
	FEvent*				WaitExecuteEvent; //等待并执行信号
	FCriticalSection	Mutex;	//锁，保证同步


	static int32		ThreadCount; //线程计数
};	