#include "Runnable/ThreadRunnableProxy.h"
#include "HAL/RunnableThread.h"

int32 FThreadRunnable::ThreadCount = 0;

FThreadRunnable::FThreadRunnable()
	:IThreadProxy()
	,bRun(false)
	,bSuspend(true) //Ĭ�ϴ�������ֱ�ӹ����̵߳�
	,Thread(nullptr)
	,ThreadEvent(FPlatformProcess::GetSynchEventFromPool()) //�õ����ǵ�Event
	,StartUpEvent(FPlatformProcess::GetSynchEventFromPool())
	,WaitExecuteEvent(FPlatformProcess::GetSynchEventFromPool())
{

}

FThreadRunnable::FThreadRunnable(bool InSuspend)
	:IThreadProxy()
	, bRun(false)
	, bSuspend(InSuspend)
	, Thread(nullptr)
	, ThreadEvent(FPlatformProcess::GetSynchEventFromPool()) //�õ����ǵ�Event
	, StartUpEvent(FPlatformProcess::GetSynchEventFromPool())
	, WaitExecuteEvent(FPlatformProcess::GetSynchEventFromPool())
{

}

FThreadRunnable::~FThreadRunnable()
{
	//�ͷ��¼�����
	FPlatformProcess::ReturnSynchEventToPool(ThreadEvent);
	FPlatformProcess::ReturnSynchEventToPool(StartUpEvent);
	FPlatformProcess::ReturnSynchEventToPool(WaitExecuteEvent);

	if (Thread != NULL)
	{
		delete Thread;
		Thread = nullptr;//���岻��
	}
}
void FThreadRunnable::CreateSafeThread()
{
	RunnableName = *FString::Printf(TEXT("SimpleThread-%i"), ThreadCount);
	Thread = FRunnableThread::Create(this, *RunnableName.ToString(), 0, TPri_BelowNormal);

	ThreadCount++;
}

void FThreadRunnable::WakeupThread() //���̵߳���ִ��
{
	ThreadEvent->Trigger();//���ѳ�˯���߳�
}

bool FThreadRunnable::IsSuspend()
{
	return bSuspend;
}

void FThreadRunnable::WaitAndCompleted()
{
	bRun = false;
	ThreadEvent->Trigger(); //����ԭ���̣߳�

	StartUpEvent->Wait(); //�������ŵ������߳� (���еȴ�����)

	//���߳�����0.03�롣�ȴ���ǰ�߳�Run()�ɹ����غ��ڼ���ִ�����̺߳�����ɾ���߳�����
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
			ThreadEvent->Wait(); //�����߳�
		}

		bSuspend = false;

		/* ҵ���߼� */
		if (ThreadDelegate.IsBound()) //�����Ƿ��
		{
			ThreadDelegate.Execute(); //ProxyInterface::ThreadDelegate
			ThreadDelegate.Unbind(); //��������
		}

		//ִ���������������߳�
		bSuspend = true;

		//�������������߳�
		WaitExecuteEvent->Trigger();

	}

	

	return 0;
}

bool FThreadRunnable::Init()
{
	if (!FPlatformProcess::SupportsMultithreading()) //��ǰƽ̨�Ƿ�֧�ֶ��߳�
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
	StartUpEvent->Trigger(); //�������߳�
	bRun = false;
}