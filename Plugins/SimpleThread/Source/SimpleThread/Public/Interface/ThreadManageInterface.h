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

//�������ɴ����̣߳��������ƣ�������ϵ��̲߳����������٣��������Ҫ�����ٴ�ʹ��
//����ͬ�����첽�Ĺ��ܣ�һ��������С������
class IThreadProxyContainer : public TArray<TSharedPtr<IThreadProxy>>, public IThreadContainer
{
protected:
	typedef TArray<TSharedPtr<IThreadProxy>> TProxyArray;

public:
	//�����̲߳���ӵ��̳߳���
	FThreadHandle operator<<(const TSharedPtr<IThreadProxy>& ThreadProxy)
	{
		MUTEX_LOCL; //��
		ThreadProxy->CreateSafeThread(); //�����߳�
		this->Add(ThreadProxy); //��ӵ��̳߳���
		return ThreadProxy->GetThreadHandle();
	}

	//���������������̣߳���û��
	FThreadHandle operator^(const FSimpleDelegate& ThreadProxy)
	{
		FThreadHandle ThreadHandle = nullptr;	//MUTEX_LOCL; ˵��������
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
			//����
			ThreadHandle = *this << MakeShareable(new FThreadRunnable(true));
		}
		return ThreadHandle;
	}

	FThreadHandle operator << (const FSimpleDelegate& ThreadProxy)
	{
		FThreadHandle ThreadHandle = nullptr;	//MUTEX_LOCL; ˵��������
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
			//����
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
							 						
//�̵߳�������������Զ����������Զ������õ��̳߳أ�ʵ�ָ�Ч�ʵ������̳߳��ص㡣(�����̳߳أ�Ҳ���������)
class IThreadTaskContainer : /*�������*/public TQueue<FSimpleDelegate>, /*�̳߳�*/public TArray<TSharedPtr<IThreadProxy>>,/*���нӿ�*/public IThreadContainer
{
protected:
	typedef TArray<TSharedPtr<IThreadProxy>>	TProxyArray;
	typedef TQueue<FSimpleDelegate>				TEventQueue;

public:
	//����������һ������
	bool operator<<(const FSimpleDelegate& ThreadProxy)
	{
		MUTEX_LOCL;
		return this->Enqueue(ThreadProxy);
	}
	//��ĩβɾ����ȡ��һ������
	bool operator>>(FSimpleDelegate& ThreadProxy)
	{
		MUTEX_LOCL;
		return this->Dequeue(ThreadProxy);
	}
	//���һ���߳�
	IThreadTaskContainer& operator<<(const TSharedPtr<IThreadProxy>& ThreadProxy)
	{
		MUTEX_LOCL;
		this->Add(ThreadProxy);
		return *this;
	}
	//�����̰߳�����
	IThreadTaskContainer& operator^(const FSimpleDelegate& ThreadProxy)
	{
		bool bSuccessful = false;
		{
			MUTEX_LOCL;
			for (auto& Tmp : *this)
			{
				if (Tmp->IsSuspend())
				{
					Tmp->GetThreadDelegate() = ThreadProxy; //��Ӵ�������
					Tmp->WakeupThread(); //����Tmp�߳�
					
					bSuccessful = true;
					break;
				}
			}
		}
		//����̳߳�û�п����̣߳�����ӵ����������
		if (!bSuccessful)
		{
			*this << ThreadProxy;
		}

		return *this;
	}
};