//创建一个线程，查看线程是不是在运行中，是不是在结束了。
//
#pragma once

#include "CoreMinimal.h"
#include "Runnable/ThreadRunnableProxy.h"
//单例
class SIMPLETHREAD_API FThreadManagement : public TSharedFromThis<FThreadManagement>
{
public:
	static TSharedRef<FThreadManagement> Get();
	static void Destroy();


public:
	template<class UserClass,typename... VarTypes>
	FThreadHandle CreateThreadRaw(UserClass* TargetClass, typename TMemFunPtrType<false, UserClass, void(VarTypes...)>::Type InMethod, VarTypes... Vars)
	{
		TSharedPtr<IThreadProxy> ThreadProxy = MakeShareable(new FThreadRunnable);
		ThreadProxy->GetThreadDelegate().BindRaw(TargetClass, InMethod, Vars...); //谁调用绑定谁的方法。、
		return UpdateThreadPool(ThreadProxy);
	}

	FThreadHandle CreateThread(const FThreadLambda& ThreadLambda);

protected:
	FThreadHandle UpdateThreadPool(TSharedPtr<IThreadProxy> ThreadProxy); //更新线程池

	 
private:
	//线程池
	TArray<TSharedPtr<IThreadProxy>> Pool; 

private:
	static TSharedPtr<FThreadManagement> ThreadManagement;
};