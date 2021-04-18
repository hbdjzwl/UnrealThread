#pragma once
#include "CoreMinimal.h"
#include "Core/SimpleThreadType.h"

class SIMPLETHREAD_API IThreadProxy : public TSharedFromThis<IThreadProxy>
{
public:
	IThreadProxy();
	virtual ~IThreadProxy() {};
	virtual void CreateSafeThread() = 0; //�����߳�
	virtual void WakeupThread() = 0; //�����߳�
	virtual bool IsSuspend() = 0; //�߳��Ƿ����
	virtual void WaitAndCompleted() = 0; //�������ŵ�ǰ�̵߳��߳�(���������߳�)
	virtual void BlockingAndCompletion() = 0;	//���������

	FORCEINLINE FSimpleDelegate& GetThreadDelegate() { return ThreadDelegate; }
	FORCEINLINE FThreadHandle GetThreadHandle() { return SimpleThreadHandle; } //ָ�뷵����ָ��(���ڱȽ��߳�α��ϣֵ)
protected:
	FSimpleDelegate ThreadDelegate; //FSimpleDelegate ue4���õĴ���
private:
	TSharedPtr<FSimpleThreadHandle> SimpleThreadHandle;
};