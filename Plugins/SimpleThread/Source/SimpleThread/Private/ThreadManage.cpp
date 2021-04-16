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

EThreadState FThreadManagement::ProceduralProgress(FThreadHandle Handle)
{
	if (!Handle.IsValid())
	{
		return EThreadState::ERROR;
	}

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
	for (auto& ThreadProxy : Pool)
	{
		//�ж�1.�Ƿ񱻹���2.�Ƿ��ǵ�ǰҪ���ѵ��̡߳�
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
	Pool.Empty();//�Զ����ýӿ������������
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
	ThreadProxy->GetThreadLambda() = ThreadLambda; //˭���ð�˭�ķ�������
	return UpdateThreadPool(ThreadProxy);
}

FThreadHandle FThreadManagement::UpdateThreadPool(TSharedPtr<IThreadProxy> ThreadProxy)
 {
	ThreadProxy->CreateSafeThread(); //�����߳�
	Pool.Add(ThreadProxy); //��ӵ��̳߳���
	
	return ThreadProxy->GetThreadHandle();
}

