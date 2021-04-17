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
	FScopeLock ScopeLock(&Mutex); //�������{....}

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
			TaskQueue.Dequeue(SimpleDelegate);//β��ɾ��һ������ȡ��
			ThreadProxy->GetThreadDelegate() = SimpleDelegate;
			ThreadProxy->WakeupThread(); //�����߳�
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

	FScopeLock ScopeLock(&Mutex); //�������{....}

	for (auto& ThreadProxy : Pool)
	{
		if (ThreadProxy->IsSuspend())
		{
			if (ThreadProxy->GetThreadHandle() == Handle)
			{
				return EThreadState::LEISURELY; //����
			}
			
		}
	}

	return EThreadState::WORKING; //����
}

bool FThreadManagement::Do(FThreadHandle Handle)
{
	FScopeLock ScopeLock(&Mutex); //�������{....}
	for (auto& ThreadProxy : Pool)
	{
		//�ж�1.�Ƿ񱻹���2.�Ƿ��ǵ�ǰҪ���ѵ��̡߳�
		if (ThreadProxy->IsSuspend()&& ThreadProxy->GetThreadHandle()==Handle)
		{
			ThreadProxy->WakeupThread();  //����ָ����˯���߳�
			return true;
		}
	}

	return false;
}

bool FThreadManagement::DoWait(FThreadHandle Handle)
{
	FScopeLock ScopeLock(&Mutex); //�������{....}

	for (auto& ThreadProxy : Pool)
	{
		//�ж�1.�Ƿ񱻹���2.�Ƿ��ǵ�ǰҪ���ѵ��̡߳�
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
	FScopeLock ScopeLock(&Mutex); //�������{....}

	for (auto& ThreadProxy : Pool)
	{
		ThreadProxy->WaitAndCompleted();
	}
	Pool.Empty();//�Զ����ýӿ������������
}

void FThreadManagement::CleanThread(FThreadHandle Handle)
{
	FScopeLock ScopeLock(&Mutex); //�������{....}

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
	FScopeLock ScopeLock(&Mutex); //�������{....}

	ThreadProxy->CreateSafeThread(); //�����߳�
	Pool.Add(ThreadProxy); //��ӵ��̳߳���
	return ThreadProxy->GetThreadHandle();
}

