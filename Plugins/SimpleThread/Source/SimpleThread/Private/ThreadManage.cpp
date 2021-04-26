#include "ThreadManage.h"
TSharedPtr<FThreadManagement> FThreadManagement::ThreadManagement = nullptr;

TSharedRef<FThreadManagement> FThreadManagement::Get()
{
	if (!ThreadManagement.IsValid())
	{
		ThreadManagement = MakeShareable(new FThreadManagement);
	}
	return ThreadManagement.ToSharedRef();
}

void FThreadManagement::Destroy()
{
	if (ThreadManagement.IsValid())
	{
		ThreadManagement = nullptr;
	}

}

/*--------------------------- FThreadTaskManagement.cpp ---------------------------*/
/*--------------------------- FThreadTaskManagement.cpp ---------------------------*/
FThreadTaskManagement::~FThreadTaskManagement()
{
	for (auto& temp : *this)
	{
		temp->WaitAndCompleted();
	}

	//下面2句不需要加，自动会清除
	//static_cast<TQueue<FSimpleDelegate>*>(this)->Empty(); // ((TQueue<FSimpleDelegate>*)this)->Empty();  //((TEvrntQueue*)this)->Empty();
	//static_cast<TArray<TSharedPtr>>*>(this)->Empty();	  // ((TArray<TSharedPtr<IThreadProxy>>*)this)->Empty(); //((TProxyArray*)this)->Empty();
}

FThreadTaskManagement::FThreadTaskManagement()
{
	Init(CPUThreadNumber);
}

void FThreadTaskManagement::Init(int32 ThreadNum)
{
	for (int32 i = 0; i < ThreadNum; i++)
	{
		*this << MakeShareable(new FThreadRunnable);
	}
}

void FThreadTaskManagement::Tick(float DeltaTime)
{
	TSharedPtr<IThreadProxy> ThreadProxy = nullptr;
	{
		MUTEX_LOCL;
		for (auto& temp : *this)
		{
			if (temp->IsSuspend())
			{
				ThreadProxy = temp;
				break;
			}
		}
	}

	if (ThreadProxy.IsValid())
	{

		//if (!(static_cast<TQueue<FSimpleDelegate>*>(this)->IsEmpty())) //TQueus和TArray
		//if (!((TEventQueue *)this)->IsEmpty())

		if( !((TQueue<FSimpleDelegate> *)this)->IsEmpty() )
		{
			FSimpleDelegate SimpleDelegate;
			if (*this >> SimpleDelegate)
			{
				MUTEX_LOCL;
				ThreadProxy->GetThreadDelegate() = SimpleDelegate;
				ThreadProxy->WakeupThread();
			}
		}
	}

}

TStatId FThreadTaskManagement::GetStatId() const
{
	return TStatId();
}


/*--------------------------- ThreadProxyManage.cpp ---------------------------*/
/*--------------------------- ThreadProxyManage.cpp ---------------------------*/

FThreadProxyManage::~FThreadProxyManage()
{
	for (auto& temp : *this)
	{
		temp->WaitAndCompleted();
	}

	//下面1句不需要加，自动会清除
	//this->Empty(); //智能指针会调用线程里析构函数
}

bool FThreadProxyManage::Join(FThreadHandle Handle) //同步
{
	TSharedPtr<IThreadProxy> ThreadProxy = *this >> Handle;

	if (ThreadProxy.IsValid())
	{
		ThreadProxy->BlockingAndCompletion(); //阻塞主线程
		return true;
	}

	return false;
}

bool FThreadProxyManage::Detach(FThreadHandle Handle) //异步
{
	TSharedPtr<IThreadProxy> ThreadProxy = *this >> Handle;

	if (ThreadProxy.IsValid())
	{
		ThreadProxy->WakeupThread();
		return true;
	}

	return false;
}

EThreadState FThreadProxyManage::Joinable(FThreadHandle Handle)
{
	TSharedPtr<IThreadProxy> ThreadProxy = *this >> Handle;
	if (ThreadProxy.IsValid())
	{
		if (ThreadProxy->IsSuspend())
		{
			return EThreadState::LEISURELY;
		}
		else
		{
			return EThreadState::WORKING;
		}
	}

	return EThreadState::ERROR;
}
