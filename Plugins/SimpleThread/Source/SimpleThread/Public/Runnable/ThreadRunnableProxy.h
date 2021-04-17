
#pragma once

#include "CoreMinimal.h"
#include "HAL/Runnable.h"
#include "Interface/ProxyInterface.h"

class FThreadRunnable : public FRunnable,public IThreadProxy
{
public:
	FThreadRunnable();
	FThreadRunnable(bool InSuspend);
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
	bool				bSuspend; //�Ƿ�����߳�
	FRunnableThread*	Thread; //�߳�ʵ��
	FName				RunnableName; //�߳�����
	FEvent*				ThreadEvent; //�߳��¼��źţ�����ǰ�̡߳�˭ִ�й���˭��(�����̻߳��ѵ�ǰ�߳�)
	FEvent*				StartUpEvent; //�������̵߳��ź�
	
	FEvent*				WaitExecuteEvent; //�ȴ���ִ���ź�
	FCriticalSection	Mutex;	//������֤ͬ��


	static int32		ThreadCount; //�̼߳���
};	