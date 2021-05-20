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
	ThreadTaskManagement.Tick(DeltaTime); //�߳��������

	CoroutinesManage.Tick(DeltaTime); //Э�̹���
}

TStatId FThreadManagement::GetStatId() const
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

	//����1�䲻��Ҫ�ӣ��Զ������
	//this->Empty(); //����ָ�������߳�����������
}

bool FThreadProxyManage::Join(FThreadHandle Handle) //ͬ��
{
	TSharedPtr<IThreadProxy> ThreadProxy = *this >> Handle;

	if (ThreadProxy.IsValid())
	{
		ThreadProxy->BlockingAndCompletion(); //�������߳�
		return true;
	}

	return false;
}

bool FThreadProxyManage::Detach(FThreadHandle Handle) //�첽
{
	TSharedPtr<IThreadProxy> ThreadProxy = *this >> Handle;

	if (ThreadProxy.IsValid())
	{
		ThreadProxy->WakeupThread(); //���ѳ�˯�߳�
		return true;
	}

	return false;
}

EThreadState FThreadProxyManage::Joinable(FThreadHandle Handle)
{
	TSharedPtr<IThreadProxy> ThreadProxy = *this >> Handle;
	if (ThreadProxy.IsValid())
	{
		if (ThreadProxy->IsSuspend()) //�߳��Ƿ����
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


/*--------------------------- FThreadTaskManagement.cpp ---------------------------*/
/*--------------------------- FThreadTaskManagement.cpp ---------------------------*/
FThreadTaskManagement::~FThreadTaskManagement()
{
	for (auto& temp : *this)
	{
		temp->WaitAndCompleted();
	}

	//����2�䲻��Ҫ�ӣ��Զ������
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
				ThreadProxy = temp; //��ȡ�����߳�
				break;
			}
		}
	}

	if (ThreadProxy.IsValid()) //��ȡ���������߳�
	{

		//if (!(static_cast<TQueue<FSimpleDelegate>*>(this)->IsEmpty())) //TQueus��TArray
		//if (!((TEventQueue *)this)->IsEmpty())

		if( !((TQueue<FSimpleDelegate> *)this)->IsEmpty() ) //������в����ڿյ�
		{
			FSimpleDelegate SimpleDelegate;
			if (*this <<= SimpleDelegate) //ȡ��������е�����
			{
				MUTEX_LOCL;
				ThreadProxy->GetThreadDelegate() = SimpleDelegate; //�����������
				ThreadProxy->WakeupThread(); //���������߳�
			}
		}
	}

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