
#pragma once

#include "CoreMinimal.h"
#include "HAL/Runnable.h"
#include "Interface/ProxyInterface.h"

class FThreadRunnable : public FRunnable,public IThreadProxy
{
public:
	FThreadRunnable();

	virtual ~FThreadRunnable();

	/* IThreadProxy�Ľӿ� */
	virtual void CreateSafeThread()override; //�����߳�
	virtual void WakeupThread()override; //�����߳�
	virtual bool IsSuspend()override; //�߳��Ƿ����
	virtual void WaitAndCompleted()override; //�������ŵ�ǰ�̵߳��߳�(���������߳�)

private:
	/* Runnable�Ľӿ� */
	virtual uint32 Run()override;
	virtual bool Init()override;
	virtual void Stop()override;
	virtual void Exit()override;


private:
	bool				bRun; //�ж��߳�ʱ����ʹ��
	bool				bSuspend; //�Ƿ�����߳�
	bool				bImplement; //�Ƿ�ִ��
	FRunnableThread*	Thread; //�߳�ʵ��
	FName				RunnableName; //�߳�����
	FEvent*				ThreadEvent; //�߳��¼�������ǰ�̡߳�˭ִ�й���˭��(�����̻߳��ѵ�ǰ�߳�)
	FEvent*				StartUpEvent; //�������̵߳�
	static int32		ThreadCount; //�̼߳���
};	