#pragma once

#include "CoreMinimal.h"
#include "Core/SimpleThreadType.h"
class SIMPLETHREAD_API IThreadProxy : public TSharedFromThis<IThreadProxy>
{
public:
	IThreadProxy();
	virtual ~IThreadProxy() {};
	virtual void CreateSafeThread() = 0; //�����߳�
	virtual void SuspendThread() = 0; //�����߳�
	virtual void WakeupThread() = 0; //�����߳�


	FORCEINLINE FSimpleDelegate& GetThreadDelegate() { return ThreadDelegate; }
	FORCEINLINE FThreadLambda& GetThreadLambda() { return ThreadLambda; }

	FORCEINLINE FThreadHandle GetThreadHandle() { return SimpleThreadHandle; } //ָ�뷵����ָ��
protected:
	FSimpleDelegate ThreadDelegate; //FSimpleDelegate ue4���õĴ���
	FThreadLambda ThreadLambda;
private:
	TSharedPtr<FSimpleThreadHandle> SimpleThreadHandle;
};