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
		ThreadManagement = nullptr;
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
	ThreadProxy->CreateSafeThread();
	Pool.Add(ThreadProxy); //��ӵ��̳߳���
	
	return ThreadProxy->GetThreadHandle();
}

