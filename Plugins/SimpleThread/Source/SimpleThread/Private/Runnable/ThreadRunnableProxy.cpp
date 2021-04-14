#include "Runnable/ThreadRunnableProxy.h"
#include "HAL/RunnableThread.h"

int32 FThreadRunnable::ThreadCount = 0;

FThreadRunnable::FThreadRunnable()
	:IThreadProxy()
	,bRun(false)
	,bSuspend(false)
	,bImplement(false)
	,Thread(nullptr)
	,ThreadEvent(FPlatformProcess::GetSynchEventFromPool()) //�õ����ǵ�Event
{

}

FThreadRunnable::~FThreadRunnable()
{
	//�ͷ��¼�����
	FPlatformProcess::ReturnSynchEventToPool(ThreadEvent);
	if (Thread != NULL)
	{

	}
}
void FThreadRunnable::CreateSafeThread()
{
	//׼��ִ���߼�
	bSuspend = true;

	RunnableName = *FString::Printf(TEXT("SimpleThread-%i"), ThreadCount);
	Thread = FRunnableThread::Create(this, *RunnableName.ToString(), 0, TPri_BelowNormal);

	ThreadCount++;
}

void FThreadRunnable::SuspendThread()
{
	bSuspend = true;

}

void FThreadRunnable::WakeupThread()
{
	bImplement = true;
	ThreadEvent->Trigger();//���ѳ�˯���߳�
}

uint32 FThreadRunnable::Run()
{
	while(bRun)
	{
		if (bSuspend)
		{
			ThreadEvent->Wait(); //�����߳�
		}

		if (bImplement)
		{
			bImplement = false;

			/* ҵ���߼� */
			if (ThreadDelegate.IsBound()) //�����Ƿ��
			{
				ThreadDelegate.Execute(); //ProxyInterface::ThreadDelegate
				ThreadDelegate.Unbind(); //��������
			}
			else
			{
				ThreadLambda(); //ProxyInterface::ThreadLambda
				ThreadLambda = nullptr; //�ÿ�Lambda
			}

			//��������߳�
			SuspendThread();
		}
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
	bRun = false;
}