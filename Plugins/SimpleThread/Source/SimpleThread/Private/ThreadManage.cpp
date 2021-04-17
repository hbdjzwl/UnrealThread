#include "ThreadManage.h"
TSharedPtr<FThreadManagement> FThreadManagement::ThreadManagement = nullptr;

TSharedRef<FThreadManagement> FThreadManagement::Get()
{
	if (!ThreadManagement.IsValid())
	{
		ThreadManagement = MakeShareable(new FThreadManagement);
	}
	return ThreadManagement.ToSharedRef();
}

void FThreadManagement::Destroy()
{
	if (ThreadManagement.IsValid())
	{
		ThreadManagement->CleanAllThread();
		ThreadManagement = nullptr;
	}

}

void FThreadManagement::Tick(float DeltaTime)
{
	FScopeLock ScopeLock(&Mutex); //区域加锁{....}

	TSharedPtr<IThreadProxy> ThreadProxy = nullptr;
	for (auto& Temp : Pool)
	{
		if (Temp->IsSuspend())
		{
			ThreadProxy = Temp;
			break;
		}
	}

	if (ThreadProxy.IsValid())
	{
		if (!TaskQueue.IsEmpty())
		{
			FSimpleDelegate SimpleDelegate;
			TaskQueue.Dequeue(SimpleDelegate);//尾部删除一个代理并取出
			ThreadProxy->GetThreadDelegate() = SimpleDelegate;
			ThreadProxy->WakeupThread(); //唤醒线程
		}
	}
}

TStatId FThreadManagement::GetStatId() const
{
	return TStatId();
}

void FThreadManagement::Init(int32 ThreadNum)
{
	for (int32 i = 0; i < ThreadNum; i++)
	{
		UpdateThreadPool(MakeShareable(new FThreadRunnable));
	}

}

EThreadState FThreadManagement::ProceduralProgress(FThreadHandle Handle)
{
	if (!Handle.IsValid())
	{
		return EThreadState::ERROR;
	}

	FScopeLock ScopeLock(&Mutex); //区域加锁{....}

	for (auto& ThreadProxy : Pool)
	{
		if (ThreadProxy->IsSuspend())
		{
			if (ThreadProxy->GetThreadHandle() == Handle)
			{
				return EThreadState::LEISURELY; //闲置
			}
			
		}
	}

	return EThreadState::WORKING; //工作
}

bool FThreadManagement::Do(FThreadHandle Handle)
{
	FScopeLock ScopeLock(&Mutex); //区域加锁{....}
	for (auto& ThreadProxy : Pool)
	{
		//判断1.是否被挂起。2.是否是当前要唤醒的线程。
		if (ThreadProxy->IsSuspend()&& ThreadProxy->GetThreadHandle()==Handle)
		{
			ThreadProxy->WakeupThread();  //唤醒指定沉睡的线程
			return true;
		}
	}

	return false;
}

bool FThreadManagement::DoWait(FThreadHandle Handle)
{
	FScopeLock ScopeLock(&Mutex); //区域加锁{....}

	for (auto& ThreadProxy : Pool)
	{
		//判断1.是否被挂起。2.是否是当前要唤醒的线程。
		if (ThreadProxy->IsSuspend() && ThreadProxy->GetThreadHandle() == Handle)
		{
			ThreadProxy->BlockingAndCompletion();
			return true;
		}
	}

	return false;
}

void FThreadManagement::CleanAllThread()
{
	FScopeLock ScopeLock(&Mutex); //区域加锁{....}

	for (auto& ThreadProxy : Pool)
	{
		ThreadProxy->WaitAndCompleted();
	}
	Pool.Empty();//自动调用接口里的析构函数
}

void FThreadManagement::CleanThread(FThreadHandle Handle)
{
	FScopeLock ScopeLock(&Mutex); //区域加锁{....}

	int32 RemoveIndex = INDEX_NONE;
	for (int32 i = 0; i < Pool.Num(); i++)
	{
		if (Pool[i]->GetThreadHandle() == Handle)
		{
			Pool[i]->WaitAndCompleted();
			RemoveIndex = i;
			break;
		}
	}
	if (RemoveIndex != INDEX_NONE)
	{
		Pool.RemoveAt(RemoveIndex);
	}
}


FThreadHandle FThreadManagement::UpdateThreadPool(TSharedPtr<IThreadProxy> ThreadProxy)
{
	FScopeLock ScopeLock(&Mutex); //区域加锁{....}

	ThreadProxy->CreateSafeThread(); //创建线程
	Pool.Add(ThreadProxy); //添加到线程池里
	return ThreadProxy->GetThreadHandle();
}

