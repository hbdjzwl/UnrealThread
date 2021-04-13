
#pragma once

#include "CoreMinimal.h"
#include "HAL/Runnable.h"
#include "Interface/ProxyInterface.h"

class FThreadRunnable : public FRunnable,public IThreadProxy
{
public:
	FThreadRunnable();

	virtual ~FThreadRunnable();

	/* IThreadProxy的接口 */
	virtual void CreateSafeThread(); //创建线程
	virtual void SuspendThread(); //挂起线程
	virtual void WakeupThread(); //创建线程

private:
	/* Runnable的接口 */
	virtual uint32 Run();
	virtual bool Init();
	virtual void Stop();
	virtual void Exit();


private:
	bool				bRun; //判断线程时候在使用
	bool				bSuspend; //是否挂起线程
	FRunnableThread*	Thread; //线程实例
	FName				RunnableName; //线程名字
	FEvent*				ThreadEvent; //线程事件，挂起当前线程。谁执行挂起谁。(其他线程唤醒当前线程)
	static int32		ThreadCount; //线程计数
};	