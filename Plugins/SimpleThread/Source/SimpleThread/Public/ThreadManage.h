//创建一个线程，查看线程是不是在运行中，是不是在结束了。
#pragma once

#include "CoreMinimal.h"
#include "Runnable/ThreadRunnableProxy.h"
//单例管理
// 主线程和其它线程都可以调用，所用需要进行加锁。
class SIMPLETHREAD_API FThreadManagement : public TSharedFromThis<FThreadManagement>
{
public:
	static TSharedRef<FThreadManagement> Get();
	static void Destroy();


public:
	//判断当前线程是否闲置中
	bool ProceduralProgress(FThreadHandle Handle);	
	bool Do(FThreadHandle Handle); //执行已经绑定好的线程，如果线程没绑定好调用Do()返回fasle;
private:
	void CleanAllThread();	//清空删除所有线程
	void CleanThread(FThreadHandle Handle);	//清除指定线程

public:
/*-------------------- 绑定方法 -------------------*/
#pragma region DelegateBind
	/*------- BinRaw --------*/
	template<class UserClass, typename... VarTypes>
	FThreadHandle BindRaw(UserClass* TargetClass, typename TMemFunPtrType<false, UserClass, void(VarTypes...)>::Type InMethod, VarTypes... Vars)
	{
		FThreadHandle Handle;
		for (auto& ThreadProxy : Poll)
		{
			if (ThreadProxy->IsSuspend()) //获取一个被挂起的线程
			{
				ThreadProxy->GetThreadDelegate().BindRaw(TargetClass, InMethod, Vars...); //绑定一个方法到闲置线程
				Handle = ThreadProxy->GetThreadHandle();
				break;
			}
		}
		if (!Handle.IsValid())
		{
			Handle = CreateThreadRaw<UserClass,VarTypes...>(TargetClass, InMethod, Vars...);
		}
		return Handle;
	}
	/*------- BinUObject --------*/
	template<class UserClass, typename... VarTypes>
	FThreadHandle BindUObject(UserClass* TargetClass, typename TMemFunPtrType<false, UserClass, void(VarTypes...)>::Type InMethod, VarTypes... Vars)
	{
		FThreadHandle Handle;
		for (auto& ThreadProxy : Poll)
		{
			if (ThreadProxy->IsSuspend()) //获取一个被挂起的线程
			{
				ThreadProxy->GetThreadDelegate().BindUObject(TargetClass, InMethod, Vars...); //绑定一个方法到闲置线程
				Handle = ThreadProxy->GetThreadHandle();
				break;
			}
		}
		if (!Handle.IsValid())
		{
			Handle = CreateThreadUObject<UserClass, VarTypes...>(TargetClass, InMethod, Vars...);
		}
		return Handle;
	}
	/*------- BindSP --------*/
	template<class UserClass, typename... VarTypes>
	FThreadHandle BindSP(UserClass* TargetClass, typename TMemFunPtrType<false, UserClass, void(VarTypes...)>::Type InMethod, VarTypes... Vars)
	{
		FThreadHandle Handle;
		for (auto& ThreadProxy : Poll)
		{
			if (ThreadProxy->IsSuspend()) //获取一个被挂起的线程
			{
				ThreadProxy->GetThreadDelegate().BindSP(TargetClass, InMethod, Vars...); //绑定一个方法到闲置线程
				Handle = ThreadProxy->GetThreadHandle();
				break;
			}
		}
		if (!Handle.IsValid())
		{
			Handle = CreateThreadSP<UserClass, VarTypes...>(TargetClass, InMethod, Vars...);
		}
		return Handle;
	}
	/*------- BindLambda --------*/
	template<class UserClass, typename... VarTypes>
	FThreadHandle BindLambda(TFunction<void(VarTypes...)> InMethod, VarTypes... Vars)
	{
		FThreadHandle Handle;
		for (auto& ThreadProxy : Poll)
		{
			if (ThreadProxy->IsSuspend()) //获取一个被挂起的线程
			{
				ThreadProxy->GetThreadDelegate().BindLambda(InMethod, Vars...); //绑定一个方法到闲置线程
				Handle = ThreadProxy->GetThreadHandle();
				break;
			}
		}
		if (!Handle.IsValid())
		{
			Handle = CreateThreadLambda <VarTypes...>( InMethod, Vars...);
		}
		return Handle;
	}

	/*------- BinUFunction --------*/
	template<class UserClass, typename... VarTypes>
	FThreadHandle BindUFunction(UserClass* TargetClass, const FName& InMethodName, VarTypes... Vars)
	{
		FThreadHandle Handle;
		for (auto& ThreadProxy : Poll)
		{
			if (ThreadProxy->IsSuspend()) //获取一个被挂起的线程
			{
				ThreadProxy->GetThreadDelegate().BindUFunction(TargetClass,InMethodName, Vars...); //绑定一个方法到闲置线程
				Handle = ThreadProxy->GetThreadHandle();
				break;
			}
		}
		if (!Handle.IsValid())
		{
			Handle = CreateThreadUFunction<UserClass,VarTypes...>(TargetClass,InMethod, Vars...);
		}
		return Handle;

	}
#pragma endregion


/*-------------------- 创建线程 + 绑定方法 -------------------*/
#pragma region DelegateCreatedAndBind
	/*------- BinRaw --------*/
	template<class UserClass,typename... VarTypes>
	FThreadHandle CreateThreadRaw(UserClass* TargetClass, typename TMemFunPtrType<false, UserClass, void(VarTypes...)>::Type InMethod, VarTypes... Vars)
	{
		TSharedPtr<IThreadProxy> ThreadProxy = MakeShareable(new FThreadRunnable);
		ThreadProxy->GetThreadDelegate().BindRaw(TargetClass, InMethod, Vars...); //谁调用绑定谁的方法。、
		return UpdateThreadPool(ThreadProxy);
	}

	/*------- BinUFunction --------*/
	template<class UserClass,typename... VarTypes>
	FThreadHandle CreateThreadUFunction(UserClass* TargetClass, const FName& InMethodName, VarTypes... Vars)
	{
		TSharedPtr<IThreadProxy> ThreadProxy = MakeShareable(new FThreadRunnable);
		ThreadProxy->GetThreadDelegate().BindUFunction(TargetClass,InMethodName,Vars...);
		return UpdateThreadPool(ThreadProxy);
	}

	/*------- BinLambda --------*/
	template<typename... VarTypes>
	FThreadHandle CreateThreadLambda(TFunction<void(VarTypes...)> InMethod, VarTypes... Vars)
	{
		TSharedPtr<IThreadProxy> ThreadProxy = MakeShareable(new FThreadRunnable);
		ThreadProxy->GetThreadDelegate().BindLambda(InMetod,Vars...);
		return UpdateThreadPool(ThreadProxy);
	}

	/*------- BinSP --------*/
	template<class UserClass, typename... VarTypes>
	FThreadHandle CtreateThreadSP(UserClass* TargetClass, typename TMemFunPtrType<false, UserClass, void(VarTypes...)>::Type InMethod, VarTypes... Vars)
	{
		TSharedPtr<IThreadProxy> ThreadProxy = MakeShareable(new FThreadRunnable);
		ThreadProxy->GetThreadDelegate().BindSP(TargetClass, InMethod, Vars...);
		return UpdateThreadPool(ThreadProxy);
	}

	/*------- BinObject --------*/
	template<class UserClass, typename... VarTypes>
	FThreadHandle CtreateThreadUObject(UserClass* TargetClass, typename TMemFunPtrType<false, UserClass, void(VarTypes...)>::Type InMethod, VarTypes... Vars)
	{
		TSharedPtr<IThreadProxy> ThreadProxy = MakeShareable(new FThreadRunnable);
		ThreadProxy->GetThreadDelegate().BindUObject(TargetClass, InMethod, Vars...);
		return UpdateThreadPool(ThreadProxy);
	}
#pragma endregion
	FThreadHandle CreateThread(const FThreadLambda& ThreadLambda);

protected:
	FThreadHandle UpdateThreadPool(TSharedPtr<IThreadProxy> ThreadProxy); //更新线程池

	 
private:
	//线程池
	TArray<TSharedPtr<IThreadProxy>> Pool; 

private:
	static TSharedPtr<FThreadManagement> ThreadManagement;
};