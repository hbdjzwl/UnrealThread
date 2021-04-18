#include "Runnable/ThreadRunnableProxy.h"
#include "HAL/RunnableThread.h"

int32 FThreadRunnable::ThreadCount = 0;

FThreadRunnable::FThreadRunnable(bool SuspendAtFirst)
	:IThreadProxy()
	,bRun(false)
	,bSuspendAtFirst(SuspendAtFirst) //Ĭ�ϴ�������ֱ�ӹ����̵߳�
	,Thread(nullptr)
{

}

FThreadRunnable::~FThreadRunnable()
{
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
	ThreadEvent.Trigger();//���ѳ�˯���߳�
}

bool FThreadRunnable::IsSuspend()
{
	return ThreadEvent.IsWait();
}

void FThreadRunnable::WaitAndCompleted()
{
	bRun = false;
	ThreadEvent.Trigger(); //����ԭ���̣߳�

	StartUpEvent.Wait(); //�������ŵ������߳� (���еȴ�����)

	//���߳�����0.03�롣�ȴ���ǰ�߳�Run()�ɹ����غ��ڼ���ִ�����̺߳�����ɾ���߳�����
	FPlatformProcess::Sleep(0.03f); 
}

void FThreadRunnable::BlockingAndCompletion()
{
	WaitExecuteEvent.Wait();
}

uint32 FThreadRunnable::Run()
{
	while(bRun)
	{
		if (!bSuspendAtFirst) //��һ���Ƿ񱻹���
		{
			ThreadEvent.Wait(); //�����߳�
		}

		/* ҵ���߼� */
		if (ThreadDelegate.IsBound()) //�����Ƿ��
		{
			ThreadDelegate.Execute(); //ProxyInterface::ThreadDelegate
			ThreadDelegate.Unbind(); //��������
		}

		//�������������߳�
		WaitExecuteEvent.Trigger();

		bSuspendAtFirst = false; //һ���Ա��������жϵ�һ���Ƿ񱻹���
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
	StartUpEvent.Trigger(); //�������߳�
	bRun = false;
}