
#pragma once

#include "CoreMinimal.h"
#include "HAL/Runnable.h"
#include "Interface/ProxyInterface.h"
#include "Core/SimpleSemaphore.h"

class FThreadRunnable : public FRunnable,public IThreadProxy
{
public:
	FThreadRunnable(bool InSuspend = false);
	virtual ~FThreadRunnable();

	/* IThreadProxy�Ľӿ� */
	virtual void CreateSafeThread()override; //�����߳�
	virtual void WakeupThread()override; //�����߳�
	virtual bool IsSuspend()override; //�߳��Ƿ����
	virtual void WaitAndCompleted()override; //�������ŵ�ǰ�̵߳��߳�(���������߳�)
	virtual void BlockingAndCompletion()override;	//���������
private:
	/* Runnable�Ľӿ� */
	virtual uint32 Run()override;
	virtual bool Init()override;
	virtual void Stop()override;
	virtual void Exit()override;


private:
	bool				bRun; //�ж��߳�ʱ����ʹ��
	bool				bSuspendAtFirst; //�Ƿ�����߳�
	FRunnableThread*	Thread; //�߳�ʵ��
	FName				RunnableName; //�߳�����

	FSimpleSemaphore	ThreadEvent; //�߳��¼��źţ�����ǰ�̡߳�˭ִ�й���˭��(�����̻߳��ѵ�ǰ�߳�)
	FSimpleSemaphore	StartUpEvent; //�������̵߳��ź�
	FSimpleSemaphore	WaitExecuteEvent; //�ȴ���ִ���ź�

	FCriticalSection	Mutex;	//������֤ͬ��


	static int32		ThreadCount; //�̼߳���
};	