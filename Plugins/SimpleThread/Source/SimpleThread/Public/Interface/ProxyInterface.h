#pragma once
#include "CoreMinimal.h"
#include "Core/SimpleThreadType.h"

class SIMPLETHREAD_API IThreadProxy : public TSharedFromThis<IThreadProxy>
{
public:
	IThreadProxy();
	virtual ~IThreadProxy() {};
	virtual void CreateSafeThread() = 0; //创建线程
	virtual void WakeupThread() = 0; //创建线程
	virtual bool IsSuspend() = 0; //线程是否挂起
	virtual void WaitAndCompleted() = 0; //启动我门当前线程的线程(可能是主线程)
	virtual void BlockingAndCompletion() = 0;	//阻塞并完成

	FORCEINLINE FSimpleDelegate& GetThreadDelegate() { return ThreadDelegate; }
	FORCEINLINE FThreadHandle GetThreadHandle() { return SimpleThreadHandle; } //指针返回弱指针(用于比较线程伪哈希值)
protected:
	FSimpleDelegate ThreadDelegate; //FSimpleDelegate ue4内置的代理
private:
	TSharedPtr<FSimpleThreadHandle> SimpleThreadHandle;
};