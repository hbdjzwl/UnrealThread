#pragma once
#include "CoreMinimal.h"
#include "Interface/ProxyInterface.h"
#include "Containers/Queue.h"
#include "Core/ThreadCoreMacro.h"
#include "Abandonable/SimpleAbandonable.h"
#include "Coroutines/SimpleCoroutines.h"
#include "Engine/StreamableManager.h"
#include "Runnable/ThreadRunnableProxy.h"


class IThreadContainer
{
public:
	IThreadContainer() {}
	~IThreadContainer() {}
protected:
	FCriticalSection Mutex;
};

//可以自由创建线程，不受限制，创建完毕的线程不会马上销毁，如果有需要可以再次使用
//具有同步和异步的功能，一般运用在小场景。
class IThreadProxyContainer : public TArray<TSharedPtr<IThreadProxy>>, public IThreadContainer
{
protected:
	typedef TArray<TSharedPtr<IThreadProxy>> TProxyArray;

public:
	//1.传入一个封装线程，并创建启动线程里内部真正的线程。
	//2.将封装线程添加到自身线程池数组里。
	IThreadProxyContainer& operator<<(const TSharedPtr<IThreadProxy>& ThreadProxy)
	{
		MUTEX_LOCL; //锁
		ThreadProxy->CreateSafeThread(); //封装线程内部创建开启线程
		this->Add(ThreadProxy); //添加到线程池里
		return *this;
	}

	//根据指定Handle获取具体线程接口类
	TSharedPtr<IThreadProxy> operator>>(const FThreadHandle& Handle)
	{
		MUTEX_LOCL;
		for (auto& temp : *this)
		{
			if (temp->GetThreadHandle() == Handle)
			{
				return temp;
			}
		}
		return NULL;
	}

	//Create 寻找一个闲置空代理线程，将一个任务代理赋予给闲置线程，没有则添加新线程(立刻执行任务)
	FThreadHandle operator>>(const FSimpleDelegate& Delegate)
	{
		FThreadHandle ThreadHandle = nullptr;

		{
			MUTEX_LOCL;//注意锁位置嵌套会死锁
			for (auto& temp : *this)
			{
				//闲置线程&&空代理
				if (temp->IsSuspend() && !temp->GetThreadDelegate().IsBound()) 
				{
					temp->GetThreadDelegate() = Delegate;//赋予任务
					ThreadHandle = temp->GetThreadHandle();
					temp->WakeupThread();//唤醒
					break;
				}
			}
		}
		//没有闲置线程:则创建新线程(赋予任务)，并添加入线程池。
		if (!ThreadHandle.IsValid())
		{
			TSharedPtr<IThreadProxy> Proxy =  MakeShareable(new FThreadRunnable(true)); 
			Proxy->GetThreadDelegate() = Delegate;
			*this << Proxy;

			ThreadHandle = Proxy->GetThreadHandle();
		}

		return ThreadHandle;
	}

	//Bind  寻找一个闲置空代理线程，将一个任务代理赋予给闲置线程，没有则添加新线程(立刻挂起线程)
	FThreadHandle operator << (const FSimpleDelegate& Delegate) //Bind
	{
		FThreadHandle ThreadHandle = nullptr;	//MUTEX_LOCL; 说会死锁。
		{
			MUTEX_LOCL;
			for (auto& temp : *this)
			{
				if (temp->IsSuspend() && !temp->GetThreadDelegate().IsBound())
				{
					temp->GetThreadDelegate() = Delegate;
					ThreadHandle = temp->GetThreadHandle();
					break;
				}
			}
		}

		if (!ThreadHandle.IsValid()) 
		{
			//1.创建一个挂起线程，2.递归调用自己
			ThreadHandle = *this << MakeShareable(new FThreadRunnable) << Delegate;	
		}
		return ThreadHandle;
	}

};
							 						
//线程的任务管理，可以自动管理任务。自动配闲置的线程池，实现高效率的利用线程池特点。(既是线程池，也是任务队列)
//个人: 固定数量的线程运行代理任务，若线程池中的线程当前无闲置线程，则会将任务代理加入任务队列中。
class IThreadTaskContainer : /*任务队列*/public TQueue<FSimpleDelegate>, /*线程池*/public TArray<TSharedPtr<IThreadProxy>>,/*公有接口*/public IThreadContainer
{
protected:
	typedef TArray<TSharedPtr<IThreadProxy>>	TProxyArray;
	typedef TQueue<FSimpleDelegate>				TEventQueue;

public:

	//任务队列添加一个任务
	void operator<<(const FSimpleDelegate& Delegate)
	{
		MUTEX_LOCL;
		this->Enqueue(Delegate);
	}

	//从任务队列末尾删除并取出一个任务
	bool operator<<=(FSimpleDelegate& Delegate)
	{
		MUTEX_LOCL;
		return this->Dequeue(Delegate);
	}

	//添加一个线程到线程池
	IThreadTaskContainer& operator<<(const TSharedPtr<IThreadProxy>& ThreadProxy)
	{
		MUTEX_LOCL;
		this->Add(ThreadProxy);
		ThreadProxy->CreateSafeThread(); //...
		return *this;
	}

	//寻找闲置未绑定的线程绑定代理任务
	void operator>>(const FSimpleDelegate& Delegate)
	{
		bool bSuccessful = false;
		{
			MUTEX_LOCL;
			for (auto& Tmp : *this)
			{
				if (Tmp->IsSuspend() && !Tmp->GetThreadDelegate().IsBound())
				{
					Tmp->GetThreadDelegate() = Delegate; //添加代理任务
					Tmp->WakeupThread(); //唤醒Tmp线程

					bSuccessful = true;
					break;
				}
			}
		}
		//如果线程池没有空余线程，则添加到任务队列中
		if (!bSuccessful)
		{
			*this << Delegate;
		}
	}
};

//同步异步线程接口
//同步异步线程接口
class IAbandonableContainer : public IThreadContainer
{
protected:
	//同步绑定代理
	void operator<<(const FSimpleDelegate& ThreadDelegate)
	{
		FAsyncTask<FSimpleAbandonable>* SimpleAbandonable = new FAsyncTask<FSimpleAbandonable>(ThreadDelegate);
		SimpleAbandonable->StartBackgroundTask(); //执行后台程序(另外线程)
		SimpleAbandonable->EnsureCompletion(); //阻塞启动线程，等待完成
		delete SimpleAbandonable;
	}

	//异步绑定代理
	void operator>>(const FSimpleDelegate& ThreadDelegate)
	{
		(new FAutoDeleteAsyncTask<FSimpleAbandonable>(ThreadDelegate))->StartBackgroundTask(); //任务完成后自动施放 
	}
};

//协程
//协程
class ICoroutinesContainer
{
public:
	ICoroutinesContainer():TmpTotalTime(0.f){}
	virtual ~ICoroutinesContainer() { ICoroutinesObject::Array.Empty(); }

	//初始化总时间
	ICoroutinesContainer &operator<<(float TotalTime)
	{
		TmpTotalTime = TotalTime;

		return *this;
	}

	//创建协程对象，并添加到协程对象数组里，返回协程容器*this
	ICoroutinesContainer &operator<<(const FSimpleDelegate& ThreadDelegate)
	{
		ICoroutinesObject::Array.Add(MakeShareable(new FCoroutinesObject(TmpTotalTime, ThreadDelegate)));

		return *this;
	}

	//创建协程对象，并添加到协程对象数组里，返回最新添加的协程对象
	FCoroutinesHandle operator>>(const FSimpleDelegate& ThreadDelegate)
	{
		ICoroutinesObject::Array.Add(MakeShareable(new FCoroutinesObject(ThreadDelegate)));

		return ICoroutinesObject::Array[ICoroutinesObject::Array.Num() - 1];
	}

	//Tick调用: 更新协程对象，并移除完成的协程对象
	void operator<<=(float Time)
	{
		TArray<TSharedPtr<ICoroutinesObject>> RemoveObject;
		for (int32 i = 0; i < ICoroutinesObject::Array.Num(); i++)
		{
			FCoroutinesRequest Request(Time); //创建一个请求对象，就是存储个Bool返回值和时间。有点多余。

			ICoroutinesObject::Array[i]->Update(Request);//每个协程对象计时增加，且条件执行。返回请求对象
			if (Request.bCompleteRequest)
			{
				RemoveObject.Add(ICoroutinesObject::Array[i]); 
			}
		}

		for (auto &RemoveInstance : RemoveObject)
		{
			ICoroutinesObject::Array.Remove(RemoveInstance); //从协程池中清理
		}
	}

private:
	float TmpTotalTime; 
};

//图表线程接口
//图表线程接口
class IGraphContainer :public IThreadContainer
{
protected:

	//呼叫主线程
	FGraphEventRef operator<<(const FSimpleDelegate& ThreadDelegate)
	{
		MUTEX_LOCL;
		return FSimpleDelegateGraphTask::CreateAndDispatchWhenReady(
			ThreadDelegate,
			TStatId(),
			nullptr,
			ENamedThreads::GameThread);
	}

	//绑定任意线程
	FGraphEventRef operator>>(const FSimpleDelegate& ThreadDelegate)
	{
		MUTEX_LOCL;
		return FSimpleDelegateGraphTask::CreateAndDispatchWhenReady(
			ThreadDelegate,
			TStatId(),
			nullptr,
			ENamedThreads::AnyThread); 
	}
};

//资源管理接口
//资源管理接口
class IStreamableContainer
{
public:
	virtual ~IStreamableContainer() {}

	//存储路径   (搜集资源路径 然后搭配 异步资源是一起用的)
	IStreamableContainer& operator>>(const TArray<FSoftObjectPath>& InObjectPath)
	{
		SetObjectPath(InObjectPath);

		return *this;
	}
	//异步资源
	TSharedPtr<struct FStreamableHandle> operator>>(const FSimpleDelegate& ThreadDelegate)
	{
		return GetStreamableManager()->RequestAsyncLoad(GetObjectPath(), ThreadDelegate); //完成异步以后执行代理
	}

	//同步读取
	TSharedPtr<struct FStreamableHandle> operator<<(const TArray<FSoftObjectPath>& InObjectPath)
	{
		return GetStreamableManager()->RequestSyncLoad(InObjectPath);
	}

protected:
	virtual void SetObjectPath(const TArray<FSoftObjectPath>& InObjectPath) = 0; //设置保存资源路径
	virtual TArray<FSoftObjectPath>& GetObjectPath() = 0; //获取资源路径
	virtual FStreamableManager* GetStreamableManager() = 0;	//管理类
};