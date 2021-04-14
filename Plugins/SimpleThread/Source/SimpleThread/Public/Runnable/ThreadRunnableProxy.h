
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
	virtual void CreateSafeThread(); //�����߳�
	virtual void SuspendThread(); //�����߳�
	virtual void WakeupThread(); //�����߳�

private:
	/* Runnable�Ľӿ� */
	virtual uint32 Run();
	virtual bool Init();
	virtual void Stop();
	virtual void Exit();


private:
	bool				bRun; //�ж��߳�ʱ����ʹ��
	bool				bSuspend; //�Ƿ�����߳�
	bool				bImplement; //�Ƿ�ִ��
	FRunnableThread*	Thread; //�߳�ʵ��
	FName				RunnableName; //�߳�����
	FEvent*				ThreadEvent; //�߳��¼�������ǰ�̡߳�˭ִ�й���˭��(�����̻߳��ѵ�ǰ�߳�)
	static int32		ThreadCount; //�̼߳���
};	