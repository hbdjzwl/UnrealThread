#include "Runnable/ThreadRunnableProxy.h"
#include "HAL/RunnableThread.h"
#include "SimpleThreadPlatform.h"  //�Զ����ƽ̨

int32 FThreadRunnable::ThreadCount = 0;

FThreadRunnable::FThreadRunnable(bool SuspendAtFirst)
	:IThreadProxy()
	,StopTaskCounter(0)
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

void FThreadRunnable::WaitAndCompleted() //ͬ��(����ʱ����)
{
	Stop();//work��while���
	if (ThreadEvent.IsWait()) //��ֹ���̱߳�����(����)��
	{
		ThreadEvent.Trigger(); //����ԭ���߳�(����Work�̱߳�����ɺ�Ż᲻����)
		StartUpEvent.Wait(); //�������ŵ������߳� (���еȴ�����)

		//���߳�����0.03�롣�ȴ���ǰ�߳�Run()�ɹ����غ��ڼ���ִ�����̺߳�����ɾ���߳�����
		FPlatformProcess::Sleep(0.03f);
	}
}

void FThreadRunnable::BlockingAndCompletion()
{
	ThreadEvent.Trigger();	 //���ѳ�˯���߳�
	WaitExecuteEvent.Wait(); //��˯�߳�
}

uint32 FThreadRunnable::Run()
{
	while(StopTaskCounter.GetValue() == 0)
	{
		if (!bSuspendAtFirst) //��һ���Ƿ񱻹������ÿ��ִ�����ϴ����񣬶��ᱻ���´ι���(����һ��������while)
		{
			ThreadEvent.Wait(); //�����߳�
		}

		/* ҵ���߼� */
		if (ThreadDelegate.IsBound()) //�����Ƿ��
		{
			ThreadDelegate.Execute(); //ProxyInterface::ThreadDelegate
			ThreadDelegate.Unbind(); //��������
		}

		WaitExecuteEvent.Trigger(); //�������������߳�

		bSuspendAtFirst = false; //һ���Ա�����ÿ��ѭ����ʼ�������
	}

	return 0;
}

bool FThreadRunnable::Init()
{
	if (!FPlatformProcess::SupportsMultithreading()) //��ǰƽ̨�Ƿ�֧�ֶ��߳�
	{
		Stop();
	}

	return StopTaskCounter.GetValue() == 0;
}
void FThreadRunnable::Stop()
{
	StopTaskCounter.Increment(); //����
}

void FThreadRunnable::Exit()
{
	StartUpEvent.Trigger(); //�������߳�
}