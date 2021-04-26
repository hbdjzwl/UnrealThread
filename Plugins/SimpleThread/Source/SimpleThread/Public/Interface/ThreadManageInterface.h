#pragma once
#include "CoreMinimal.h"
#include "Interface/ProxyInterface.h"
#include "Containers/Queue.h"
#include "Core/ThreadCoreMacro.h"

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
	//创建线程并添加到线程池中
	FThreadHandle operator<<(const TSharedPtr<IThreadProxy>& ThreadProxy)
	{
		MUTEX_LOCL; //锁
		ThreadProxy->CreateSafeThread(); //创建线程
		this->Add(ThreadProxy); //添加到线程池里
		return ThreadProxy->GetThreadHandle();
	}

	//添加任务代理到闲置线程，若没有
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

		if (!ThreadHandle.IsValid())
		{
			//创建
			ThreadHandle = *this << MakeShareable(new FThreadRunnable(true));
		}
		return ThreadHandle;
	}

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
			ThreadHandle = *this << MakeShareable(new FThreadRunnable);
		}
		return ThreadHandle;
	}

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
	//从末尾删除并取出一个任务
	bool operator>>(FSimpleDelegate& ThreadProxy)
	{
		MUTEX_LOCL;
		return this->Dequeue(ThreadProxy);
	}
	//添加一个线程
	IThreadTaskContainer& operator<<(const TSharedPtr<IThreadProxy>& ThreadProxy)
	{
		MUTEX_LOCL;
		this->Add(ThreadProxy);
		return *this;
	}
	//空闲线程绑定任务
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