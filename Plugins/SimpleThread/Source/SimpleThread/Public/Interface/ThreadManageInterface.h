#pragma once
#include "CoreMinimal.h"
#include "Interface/ProxyInterface.h"
#include "Containers/Queue.h"
#include "Core/ThreadCoreMacro.h"
#include "Abandonable/SimpleAbandonable.h"

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
	//1.传入一个封装线程，并创建封装线程里内部真正的线程添加到线程池中
	//2.将封装线程添加到自身数组里。
	FThreadHandle operator<<(const TSharedPtr<IThreadProxy>& ThreadProxy)
	{
		MUTEX_LOCL; //锁
		ThreadProxy->CreateSafeThread(); //创建线程
		this->Add(ThreadProxy); //添加到线程池里
		return ThreadProxy->GetThreadHandle();
	}

	//寻找一个闲置线程，将一个绑定任务的代理设置给线程，若线程不够，才开辟新线程，并返回线程Handle
	FThreadHandle operator^(const FSimpleDelegate& ThreadProxy)
	{
		FThreadHandle ThreadHandle = nullptr;	//MUTEX_LOCL; 说会死锁。
		{
			MUTEX_LOCL;
			for (auto& temp : *this)
			{
				if (temp->IsSuspend())
				{
					temp->GetThreadDelegate() = ThreadProxy;
					ThreadHandle = temp->GetThreadHandle();
					break;
				}
			}
		}

		//线程数量不够，则创建新的线程添加线程池中
		if (!ThreadHandle.IsValid())
		{
			ThreadHandle = *this << MakeShareable(new FThreadRunnable(true)); //创建既执行
		}
		return ThreadHandle;
	}

	//与FThreadHandle operator^相同。唯独就是new FThreadRunnable()参数不同
	FThreadHandle operator << (const FSimpleDelegate& ThreadProxy)
	{
		FThreadHandle ThreadHandle = nullptr;	//MUTEX_LOCL; 说会死锁。
		{
			MUTEX_LOCL;
			for (auto& temp : *this)
			{
				if (temp->IsSuspend())
				{
					temp->GetThreadDelegate() = ThreadProxy;
					ThreadHandle = temp->GetThreadHandle();
					break;
				}
			}
		}

		if (!ThreadHandle.IsValid())
		{
			//创建
			ThreadHandle = *this << MakeShareable(new FThreadRunnable);	//与FThreadHandle operator^相同。唯独就是new FThreadRunnable()参数不同
		}
		return ThreadHandle;
	}

	//根据Handle返回具体线程接口类
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
};
							 						
//线程的任务管理，可以自动管理任务。自动配闲置的线程池，实现高效率的利用线程池特点。(既是线程池，也是任务队列)
// 
//个人: 固定数量的线程运行代理任务，若线程池中的线程当前无闲置线程，则会将任务代理加入任务队列中。
class IThreadTaskContainer : /*任务队列*/public TQueue<FSimpleDelegate>, /*线程池*/public TArray<TSharedPtr<IThreadProxy>>,/*公有接口*/public IThreadContainer
{
protected:
	typedef TArray<TSharedPtr<IThreadProxy>>	TProxyArray;
	typedef TQueue<FSimpleDelegate>				TEventQueue;

public:

	//任务队列添加一个任务
	bool operator<<(const FSimpleDelegate& ThreadProxy)
	{
		MUTEX_LOCL;
		return this->Enqueue(ThreadProxy);
	}

	//从任务队列末尾删除并取出一个任务
	bool operator>>(FSimpleDelegate& ThreadProxy)
	{
		MUTEX_LOCL;
		return this->Dequeue(ThreadProxy);
	}

	//添加一个线程到线程池
	IThreadTaskContainer& operator<<(const TSharedPtr<IThreadProxy>& ThreadProxy)
	{
		MUTEX_LOCL;
		this->Add(ThreadProxy);
		return *this;
	}

	//寻找闲置线程绑定代理任务
	IThreadTaskContainer& operator^(const FSimpleDelegate& ThreadProxy)
	{
		bool bSuccessful = false;
		{
			MUTEX_LOCL;
			for (auto& Tmp : *this)
			{
				if (Tmp->IsSuspend())
				{
					Tmp->GetThreadDelegate() = ThreadProxy; //添加代理任务
					Tmp->WakeupThread(); //唤醒Tmp线程

					bSuccessful = true;
					break;
				}
			}
		}
		//如果线程池没有空余线程，则添加到任务队列中
		if (!bSuccessful)
		{
			*this << ThreadProxy;
		}

		return *this;
	}
};

//同步异步线程接口
class IAbandonableContatiner : public IThreadContainer
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