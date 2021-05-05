#pragma once
#include "CoreMinimal.h"
#include "Interface/ProxyInterface.h"
#include "Containers/Queue.h"
#include "Core/ThreadCoreMacro.h"
#include "Abandonable/SimpleAbandonable.h"
#include "Coroutines/SimpleCoroutines.h"

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
	IThreadProxyContainer& operator<<(const TSharedPtr<IThreadProxy>& ThreadProxy)
	{
		MUTEX_LOCL; //��
		ThreadProxy->CreateSafeThread(); //�����߳�
		this->Add(ThreadProxy); //��ӵ��̳߳���
		return *this;
	}

	//Ѱ��һ�����ÿմ����̣߳���һ��������Ĵ������ø��̣߳����̲߳������ſ������̣߳��������߳�Handle
	FThreadHandle operator>>(const FSimpleDelegate& ThreadProxy)
	{
		FThreadHandle ThreadHandle = nullptr;	//MUTEX_LOCL; ˵��������
		{
			MUTEX_LOCL;
			for (auto& temp : *this)
			{
				if (temp->IsSuspend() && !temp->GetThreadDelegate().IsBound()) //�����߳�&&�մ���
				{
					temp->GetThreadDelegate() = ThreadProxy;
					ThreadHandle = temp->GetThreadHandle();
					break;
				}
			}
		}

		//1.�߳������������򴴽��µ��߳�����̳߳��С�2.����������
		if (!ThreadHandle.IsValid())
		{
			ThreadHandle = *this << MakeShareable(new FThreadRunnable(true)) >> ThreadProxy; //������ִ��
		}
		return ThreadHandle;
	}

	//��FThreadHandle operator>>��ͬ��Ψ������new FThreadRunnable()������ͬ
	FThreadHandle operator << (const FSimpleDelegate& ThreadProxy)
	{
		FThreadHandle ThreadHandle = nullptr;	//MUTEX_LOCL; ˵��������
		{
			MUTEX_LOCL;
			for (auto& temp : *this)
			{
				if (temp->IsSuspend() && !temp->GetThreadDelegate().IsBound())
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
			ThreadHandle = *this << MakeShareable(new FThreadRunnable) << ThreadProxy;	//��FThreadHandle operator^��ͬ��Ψ������new FThreadRunnable()������ͬ
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
//����: �̶��������߳����д����������̳߳��е��̵߳�ǰ�������̣߳���Ὣ������������������С�
class IThreadTaskContainer : /*�������*/public TQueue<FSimpleDelegate>, /*�̳߳�*/public TArray<TSharedPtr<IThreadProxy>>,/*���нӿ�*/public IThreadContainer
{
protected:
	typedef TArray<TSharedPtr<IThreadProxy>>	TProxyArray;
	typedef TQueue<FSimpleDelegate>				TEventQueue;

public:

	//����������һ������
	void operator<<(const FSimpleDelegate& ThreadProxy)
	{
		MUTEX_LOCL;
		this->Enqueue(ThreadProxy);
	}

	//���������ĩβɾ����ȡ��һ������
	bool operator<<=(FSimpleDelegate& ThreadProxy)
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

	//Ѱ������δ�󶨵��̰߳󶨴�������
	void operator>>(const FSimpleDelegate& ThreadProxy)
	{
		bool bSuccessful = false;
		{
			MUTEX_LOCL;
			for (auto& Tmp : *this)
			{
				if (Tmp->IsSuspend() && !Tmp->GetThreadDelegate().IsBound())
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
	}
};

//ͬ���첽�߳̽ӿ�
class IAbandonableContainer : public IThreadContainer
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

//Э��
class ICoroutinesContainer
{
public:
	ICoroutinesContainer():TmpTotalTime(0.f){}
	virtual ~ICoroutinesContainer() { ICoroutinesObject::Array.Empty(); }

	//��ʼ����ʱ��
	ICoroutinesContainer &operator<<(float TotalTime)
	{
		TmpTotalTime = TotalTime;

		return *this;
	}

	//��������̶߳��󣬷���*this
	ICoroutinesContainer &operator<<(const FSimpleDelegate& ThreadDelegate)
	{
		ICoroutinesObject::Array.Add(MakeShareable(new FCoroutinesObject(TmpTotalTime, ThreadDelegate)));

		return *this;
	}

	//�Ƴ���ɵ�Э�̶���
	void operator<<=(float Time)
	{
		TArray<TSharedPtr<ICoroutinesObject>> RemoveObject;
		for (int32 i = 0; i < ICoroutinesObject::Array.Num(); i++)
		{
			FCoroutinesRequest Request(Time);

			ICoroutinesObject::Array[i]->Update(Request);
			if (Request.bCompleteRequest)
			{
				RemoveObject.Add(ICoroutinesObject::Array[i]);
			}
		}

		for (auto &RemoveInstance : RemoveObject)
		{
			ICoroutinesObject::Array.Remove(RemoveInstance);
		}
	}

	//��Ӵ���Э�̶��󣬷���������ӵ�Э�̶���
	FCoroutinesHandle operator>>(const FSimpleDelegate& ThreadDelegate)
	{
		ICoroutinesObject::Array.Add(MakeShareable(new FCoroutinesObject(ThreadDelegate)));

		return ICoroutinesObject::Array[ICoroutinesObject::Array.Num() - 1];
	}
private:
	float TmpTotalTime; 
};