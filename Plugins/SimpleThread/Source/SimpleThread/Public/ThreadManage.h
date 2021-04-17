//创建一个线程，查看线程是不是在运行中，是不是在结束了。
#pragma once

#include "CoreMinimal.h"
#include "Runnable/ThreadRunnableProxy.h"
#include "Tickable.h"

#if PLATFORM_WINDOWS
#include <iostream>
#include <thread>
#define CPUThreadNumber std::thread::hardware_concurrency()
#else
#define CPUThreadNumber 12
#endif

enum class EThreadState
{
	LEISURELY,
	WORKING,
	ERROR
};


//FThreadManagement 是线程安全的，不容易出现死锁。
//单例模式: 主线程和其它线程都可以调用，所用需要进行加锁。
class SIMPLETHREAD_API FThreadManagement : public TSharedFromThis<FThreadManagement>, public FTickableGameObject
{
public:
	static TSharedRef<FThreadManagement> Get();
	static void Destroy();
	
private:
	virtual void Tick(float DeltaTime)override;
	virtual TStatId GetStatId()const override;

public:
	//初始化生成挂起线程
	void Init(int32 ThreadNum);

	//判断当前线程是否闲置中
	EThreadState ProceduralProgress(FThreadHandle Handle);

	//执行已经绑定好的线程，如果线程没绑定好调用Do()返回fasle;
	//激活线程，不阻塞激活线程的线程(异步)
	//使用该方法需要和绑定之间要有一帧左右的时间间隔。
	bool Do(FThreadHandle Handle); 
	
	//激活线程，阻塞激活线程的线程，直到子线程任务完成(同步)
	//使用该方法需要和绑定之间要有一帧左右的时间间隔。
	bool DoWait(FThreadHandle Handle);
private:
	void CleanAllThread();	//清空删除所有线程
	void CleanThread(FThreadHandle Handle);	//清除指定线程

public:
/*----------------------- 已有线程绑定方法 ----------------------*/
#pragma region Bind

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

	template<class UserClass, typename... VarTypes>
	FThreadHandle BindSP(const TSharedRef<UserClass>& TargetClass, typename TMemFunPtrType<false, UserClass, void(VarTypes...)>::Type InMethod, VarTypes... Vars)
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

	template<typename FuncType, typename... VarTypes>
	FThreadHandle BindLambda(FuncType&& InMethod, VarTypes... Vars)
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

/*----------------------- 创建线程(绑定方法) ----------------------*/
#pragma region CreatedThread
	template<class UserClass, typename... VarTypes>
	FThreadHandle CreateRaw(UserClass* TargetClass, typename TMemFunPtrType<false, UserClass, void(VarTypes...)>::Type InMethod, VarTypes... Vars)
	{
		if (Pool.Num() < CPUThreadNumber)
		{
			TSharedPtr<IThreadProxy> ThreadProxy = MakeShareable(new FThreadRunnable(false));
			ThreadProxy->GetThreadDelegate().BindRaw(TargetClass, InMethod, Vars...); //谁调用绑定谁的方法。、
			return UpdateThreadPool(ThreadProxy);
		}
		else
		{
			AddRawToQueue(TargetClass,InMethod,Vars...);
		}
		return nullptr;
	}

	template<class UserClass, typename... VarTypes>
	FThreadHandle CreateUFunction(UserClass* TargetClass, const FName& InMethodName, VarTypes... Vars)
	{
		TSharedPtr<IThreadProxy> ThreadProxy = MakeShareable(new FThreadRunnable(false));
		ThreadProxy->GetThreadDelegate().BindUFunction(TargetClass, InMethodName, Vars...);
		return UpdateThreadPool(ThreadProxy);
	}

	template<typename FuncType, typename... VarTypes>
	FThreadHandle CreateLambda(FuncType&& InMethod, VarTypes... Vars)
	{
		TSharedPtr<IThreadProxy> ThreadProxy = MakeShareable(new FThreadRunnable(false));
		ThreadProxy->GetThreadDelegate().BindLambda(InMetod, Vars...);
		return UpdateThreadPool(ThreadProxy);
	}

	template<class UserClass, typename... VarTypes>
	FThreadHandle CtreateSP(const TSharedRef<UserClass>& TargetClass, typename TMemFunPtrType<false, UserClass, void(VarTypes...)>::Type InMethod, VarTypes... Vars)
	{
		TSharedPtr<IThreadProxy> ThreadProxy = MakeShareable(new FThreadRunnable(false));
		ThreadProxy->GetThreadDelegate().BindSP(TargetClass, InMethod, Vars...);
		return UpdateThreadPool(ThreadProxy);
	}

	template<class UserClass, typename... VarTypes>
	FThreadHandle CtreateUObject(UserClass* TargetClass, typename TMemFunPtrType<false, UserClass, void(VarTypes...)>::Type InMethod, VarTypes... Vars)
	{
		TSharedPtr<IThreadProxy> ThreadProxy = MakeShareable(new FThreadRunnable(false));
		ThreadProxy->GetThreadDelegate().BindUObject(TargetClass, InMethod, Vars...);
		return UpdateThreadPool(ThreadProxy);
	}
#pragma endregion

/*-------------------- 创建挂起线程(绑定方法) -------------------*/
#pragma region CreatedSuspendThread

	template<class UserClass,typename... VarTypes>
	FThreadHandle CreateThreadRaw(UserClass* TargetClass, typename TMemFunPtrType<false, UserClass, void(VarTypes...)>::Type InMethod, VarTypes... Vars)
	{
		TSharedPtr<IThreadProxy> ThreadProxy = MakeShareable(new FThreadRunnable);
		ThreadProxy->GetThreadDelegate().BindRaw(TargetClass, InMethod, Vars...); //谁调用绑定谁的方法。、
		return UpdateThreadPool(ThreadProxy);
	}

	template<class UserClass,typename... VarTypes>
	FThreadHandle CreateThreadUFunction(UserClass* TargetClass, const FName& InMethodName, VarTypes... Vars)
	{
		TSharedPtr<IThreadProxy> ThreadProxy = MakeShareable(new FThreadRunnable);
		ThreadProxy->GetThreadDelegate().BindUFunction(TargetClass,InMethodName,Vars...);
		return UpdateThreadPool(ThreadProxy);
	}

	template<typename FuncType,typename... VarTypes>
	FThreadHandle CreateThreadLambda(FuncType&& InMethod, VarTypes... Vars)
	{
		TSharedPtr<IThreadProxy> ThreadProxy = MakeShareable(new FThreadRunnable);
		ThreadProxy->GetThreadDelegate().BindLambda(InMetod,Vars...);
		return UpdateThreadPool(ThreadProxy);
	}

	template<class UserClass, typename... VarTypes>
	FThreadHandle CtreateThreadSP(const TSharedRef<UserClass>& TargetClass, typename TMemFunPtrType<false, UserClass, void(VarTypes...)>::Type InMethod, VarTypes... Vars)
	{
		TSharedPtr<IThreadProxy> ThreadProxy = MakeShareable(new FThreadRunnable);
		ThreadProxy->GetThreadDelegate().BindSP(TargetClass, InMethod, Vars...);
		return UpdateThreadPool(ThreadProxy);
	}

	template<class UserClass, typename... VarTypes>
	FThreadHandle CtreateThreadUObject(UserClass* TargetClass, typename TMemFunPtrType<false, UserClass, void(VarTypes...)>::Type InMethod, VarTypes... Vars)
	{
		TSharedPtr<IThreadProxy> ThreadProxy = MakeShareable(new FThreadRunnable);
		ThreadProxy->GetThreadDelegate().BindUObject(TargetClass, InMethod, Vars...);
		return UpdateThreadPool(ThreadProxy);
	}
#pragma endregion

private:
	template<class UserClass,typename... VarTypes>
	void AddRawToQueue(UserClass* TargetClass, typename TMemFunPtrType<false, UserClass, void(VarTypes...)>::Type InMethod, VarTypes... Vars)
	{
		FScopeLock ScopeLock(&Mutex); //加锁
		TaskQueue.Enqueue(FSimpleDelegate::CreateRaw(TargetClass,InMethod,Var...));	//动态创建新代理
	}

	template<class UserClass, typename... VarTypes>
	FThreadHandle AddUFunctionToQueue(UserClass* TargetClass, const FName& InMethodName, VarTypes... Vars)
	{
		FScopeLock ScopeLock(&Mutex); //加锁
		TaskQueue.Enqueue(FSimpleDelegate::CreateUFunction(TargetClass, InMethodName, Var...));	//动态创建新代理
	}

	template<typename FuncType, typename... VarTypes>
	FThreadHandle AddLambdaToQueue(FuncType&& InMethod, VarTypes... Vars)
	{
		FScopeLock ScopeLock(&Mutex); //加锁
		TaskQueue.Enqueue(FSimpleDelegate::CreateLambda(InMethod, Var...));	//动态创建新代理
	}
	template<class UserClass, typename... VarTypes>
	FThreadHandle AddSPToQueue(const TSharedRef<UserClass>& TargetClass, typename TMemFunPtrType<false, UserClass, void(VarTypes...)>::Type InMethod, VarTypes... Vars)
	{
		FScopeLock ScopeLock(&Mutex); //加锁
		TaskQueue.Enqueue(FSimpleDelegate::CreateSP(TargetClass, InMethod, Var...));	//动态创建新代理
	}
	template<class UserClass, typename... VarTypes>
	FThreadHandle AddUObjectToQueue(UserClass* TargetClass, typename TMemFunPtrType<false, UserClass, void(VarTypes...)>::Type InMethod, VarTypes... Vars)
	{
		FScopeLock ScopeLock(&Mutex); //加锁
		TaskQueue.Enqueue(FSimpleDelegate::CreateUObject(TargetClass, InMethod, Var...));	//动态创建新代理
	}
protected:
	FThreadHandle UpdateThreadPool(TSharedPtr<IThreadProxy> ThreadProxy); //更新线程池
 
private:
	TArray<TSharedPtr<IThreadProxy>> Pool;	//线程池
	TQueue<FSimpleDelegate> TaskQueue;	//任务队列 #include "Queue"
	FCriticalSection Mutex;	//区域锁
private:
	static TSharedPtr<FThreadManagement> ThreadManagement;
};