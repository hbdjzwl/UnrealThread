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

//�������ɴ����̣߳��������ƣ�������ϵ��̲߳����������٣��������Ҫ�����ٴ�ʹ��
//����ͬ�����첽�Ĺ��ܣ�һ��������С������
class IThreadProxyContainer : public TArray<TSharedPtr<IThreadProxy>>, public IThreadContainer
{
protected:
	typedef TArray<TSharedPtr<IThreadProxy>> TProxyArray;

public:
	//1.����һ����װ�̣߳���������װ�߳����ڲ��������߳���ӵ��̳߳���
	//2.����װ�߳���ӵ����������
	FThreadHandle operator<<(const TSharedPtr<IThreadProxy>& ThreadProxy)
	{
		MUTEX_LOCL; //��
		ThreadProxy->CreateSafeThread(); //�����߳�
		this->Add(ThreadProxy); //��ӵ��̳߳���
		return ThreadProxy->GetThreadHandle();
	}

	//Ѱ��һ�������̣߳���һ��������Ĵ������ø��̣߳����̲߳������ſ������̣߳��������߳�Handle
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

		//�߳������������򴴽��µ��߳�����̳߳���
		if (!ThreadHandle.IsValid())
		{
			ThreadHandle = *this << MakeShareable(new FThreadRunnable(true)); //������ִ��
		}
		return ThreadHandle;
	}

	//��FThreadHandle operator^��ͬ��Ψ������new FThreadRunnable()������ͬ
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
			ThreadHandle = *this << MakeShareable(new FThreadRunnable);	//��FThreadHandle operator^��ͬ��Ψ������new FThreadRunnable()������ͬ
		}
		return ThreadHandle;
	}

	//����Handle���ؾ����߳̽ӿ���
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
// 
//����: �̶��������߳����д����������̳߳��е��̵߳�ǰ�������̣߳���Ὣ������������������С�
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

	//���������ĩβɾ����ȡ��һ������
	bool operator>>(FSimpleDelegate& ThreadProxy)
	{
		MUTEX_LOCL;
		return this->Dequeue(ThreadProxy);
	}

	//���һ���̵߳��̳߳�
	IThreadTaskContainer& operator<<(const TSharedPtr<IThreadProxy>& ThreadProxy)
	{
		MUTEX_LOCL;
		this->Add(ThreadProxy);
		return *this;
	}

	//Ѱ�������̰߳󶨴�������
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

//ͬ���첽�߳̽ӿ�
class IAbandonableContatiner : public IThreadContainer
{
protected:
	//ͬ���󶨴���
	void operator<<(const FSimpleDelegate& ThreadDelegate)
	{
		FAsyncTask<FSimpleAbandonable>* SimpleAbandonable = new FAsyncTask<FSimpleAbandonable>(ThreadDelegate);
		SimpleAbandonable->StartBackgroundTask(); //ִ�к�̨����(�����߳�)
		SimpleAbandonable->EnsureCompletion(); //���������̣߳��ȴ����
		delete SimpleAbandonable;
	}

	//�첽�󶨴���
	void operator>>(const FSimpleDelegate& ThreadDelegate)
	{
		(new FAutoDeleteAsyncTask<FSimpleAbandonable>(ThreadDelegate))->StartBackgroundTask(); //������ɺ��Զ�ʩ�� 
	}
};