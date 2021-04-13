#pragma once

#include "CoreMinimal.h"
#include "Core/SimpleThreadType.h"
class SIMPLETHREAD_API IThreadProxy : public TSharedFromThis<IThreadProxy>
{
public:
	IThreadProxy();
	virtual ~IThreadProxy() {};
	virtual void CreateSafeThread() = 0; //创建线程
	virtual void SuspendThread() = 0; //挂起线程
	virtual void WakeupThread() = 0; //创建线程


	FORCEINLINE FSimpleDelegate& GetThreadDelegate() { return ThreadDelegate; }
	FORCEINLINE FThreadLambda& GetThreadLambda() { return ThreadLambda; }

	FORCEINLINE FThreadHandle GetThreadHandle() { return SimpleThreadHandle; } //指针返回弱指针
protected:
	FSimpleDelegate ThreadDelegate; //FSimpleDelegate ue4内置的代理
	FThreadLambda ThreadLambda;
private:
	TSharedPtr<FSimpleThreadHandle> SimpleThreadHandle;
};