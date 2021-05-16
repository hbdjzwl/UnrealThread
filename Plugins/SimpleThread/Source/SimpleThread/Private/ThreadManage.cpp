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

void FThreadManagement::Tick(float DeltaTime)
{
	ThreadTaskManagement.Tick(DeltaTime); //线程任务队列

	CoroutinesManage.Tick(DeltaTime); //协程管理
}

TStatId FThreadManagement::GetStatId() const
{
	return TStatId();
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
	FPlatformProcess::Sleep(0.1f);
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
				ThreadProxy = temp; //获取闲置线程
				break;
			}
		}
	}

	if (ThreadProxy.IsValid()) //获取到了限制线程
	{

		//if (!(static_cast<TQueue<FSimpleDelegate>*>(this)->IsEmpty())) //TQueus和TArray
		//if (!((TEventQueue *)this)->IsEmpty())

		if( !((TQueue<FSimpleDelegate> *)this)->IsEmpty() ) //任务队列不等于空的
		{
			FSimpleDelegate SimpleDelegate;
			if (*this <<= SimpleDelegate) //取任务队列中的任务
			{
				MUTEX_LOCL;
				ThreadProxy->GetThreadDelegate() = SimpleDelegate; //设置任务代理
				ThreadProxy->WakeupThread(); //唤醒闲置线程
			}
		}
	}

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
		ThreadProxy->WakeupThread(); //唤醒沉睡线程
		return true;
	}

	return false;
}

EThreadState FThreadProxyManage::Joinable(FThreadHandle Handle)
{
	TSharedPtr<IThreadProxy> ThreadProxy = *this >> Handle;
	if (ThreadProxy.IsValid())
	{
		if (ThreadProxy->IsSuspend()) //线程是否挂起
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


/*--------------------------- FCoroutinesManage.cpp ---------------------------*/
/*--------------------------- FCoroutinesManage.cpp ---------------------------*/

FCoroutinesManage::FCoroutinesManage()
	:FThreadTemplateBase<ICoroutinesContainer, FCoroutinesHandle>()
{

}

void FCoroutinesManage::Tick(float DeltaTime)
{
	*this <<= DeltaTime;
}

/*--------------------------- FThreadGraphManage.cpp ---------------------------*/
/*--------------------------- FThreadGraphManage.cpp ---------------------------*/
void FThreadGraphManage::Wait(const FGraphEventRef &EventRef)
{
	FTaskGraphInterface::Get().WaitUntilTaskCompletes(EventRef);
}

void FThreadGraphManage::Wait(const FGraphEventArray &EventRef)
{
	FTaskGraphInterface::Get().WaitUntilTasksComplete(EventRef);
}


/*--------------------------- FResourceLoadingManage.cpp ---------------------------*/
/*--------------------------- FResourceLoadingManage.cpp ---------------------------*/
FResourceLoadingManage::FResourceLoadingManage()
	:Super()
	, StreamableManager(new FStreamableManager())
{

}

FResourceLoadingManage::~FResourceLoadingManage()
{
	delete StreamableManager;
}

void FResourceLoadingManage::SetObjectPath(const TArray<FSoftObjectPath>& InObjectPath)
{
	SoftObjectPath = InObjectPath;
}

TArray<FSoftObjectPath>& FResourceLoadingManage::GetObjectPath()
{
	return SoftObjectPath;
}

FStreamableManager* FResourceLoadingManage::GetStreamableManager()
{
	return StreamableManager;
}