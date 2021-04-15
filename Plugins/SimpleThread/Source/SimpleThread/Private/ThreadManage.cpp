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

bool FThreadManagement::ProceduralProgress(FThreadHandle Handle)
{

	return false;
}

bool FThreadManagement::Do(FThreadHandle Handle)
{
	for (auto& ThreadProxy : Pool)
	{
		//判断1.是否被挂起。2.是否是当前要唤醒的线程。
		if (ThreadProxy->IsSuspend()&& ThreadProxy->GetThreadHandle()==Handle)
		{
			ThreadProxy->WakeupThread(); 
			return true;
		}
	}

	return false;
}

void FThreadManagement::CleanAllThread()
{
	for (auto& ThreadProxy : Pool)
	{
		ThreadProxy->WaitAndCompleted();
	}
	Pool.Empty();//自动调用接口里的析构函数
}

void FThreadManagement::CleanThread(FThreadHandle Handle)
{
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



FThreadHandle FThreadManagement::CreateThread(const FThreadLambda& ThreadLambda)
{
	TSharedPtr<IThreadProxy> ThreadProxy = MakeShareable(new FThreadRunnable);
	ThreadProxy->GetThreadLambda() = ThreadLambda; //谁调用绑定谁的方法。、
	return UpdateThreadPool(ThreadProxy);
}

FThreadHandle FThreadManagement::UpdateThreadPool(TSharedPtr<IThreadProxy> ThreadProxy)
 {
	ThreadProxy->CreateSafeThread(); //创建线程
	Pool.Add(ThreadProxy); //添加到线程池里
	
	return ThreadProxy->GetThreadHandle();
}

