//����һ���̣߳��鿴�߳��ǲ����������У��ǲ����ڽ����ˡ�
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


//FThreadManagement ���̰߳�ȫ�ģ������׳���������
//����ģʽ: ���̺߳������̶߳����Ե��ã�������Ҫ���м�����
class SIMPLETHREAD_API FThreadManagement : public TSharedFromThis<FThreadManagement>, public FTickableGameObject
{
public:
	static TSharedRef<FThreadManagement> Get();
	static void Destroy();
	
private:
	virtual void Tick(float DeltaTime)override;
	virtual TStatId GetStatId()const override;

public:
	//��ʼ�����ɹ����߳�
	void Init(int32 ThreadNum);

	//�жϵ�ǰ�߳��Ƿ�������
	EThreadState ProceduralProgress(FThreadHandle Handle);

	//ִ���Ѿ��󶨺õ��̣߳�����߳�û�󶨺õ���Do()����fasle;
	//�����̣߳������������̵߳��߳�(�첽)
	//ʹ�ø÷�����Ҫ�Ͱ�֮��Ҫ��һ֡���ҵ�ʱ������
	bool Do(FThreadHandle Handle); 
	
	//�����̣߳����������̵߳��̣߳�ֱ�����߳��������(ͬ��)
	//ʹ�ø÷�����Ҫ�Ͱ�֮��Ҫ��һ֡���ҵ�ʱ������
	bool DoWait(FThreadHandle Handle);
private:
	void CleanAllThread();	//���ɾ�������߳�
	void CleanThread(FThreadHandle Handle);	//���ָ���߳�

public:
/*----------------------- �����̰߳󶨷��� ----------------------*/
#pragma region Bind

	template<class UserClass, typename... VarTypes>
	FThreadHandle BindRaw(UserClass* TargetClass, typename TMemFunPtrType<false, UserClass, void(VarTypes...)>::Type InMethod, VarTypes... Vars)
	{
		FThreadHandle Handle;
		for (auto& ThreadProxy : Poll)
		{
			if (ThreadProxy->IsSuspend()) //��ȡһ����������߳�
			{
				ThreadProxy->GetThreadDelegate().BindRaw(TargetClass, InMethod, Vars...); //��һ�������������߳�
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
			if (ThreadProxy->IsSuspend()) //��ȡһ����������߳�
			{
				ThreadProxy->GetThreadDelegate().BindUObject(TargetClass, InMethod, Vars...); //��һ�������������߳�
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
			if (ThreadProxy->IsSuspend()) //��ȡһ����������߳�
			{
				ThreadProxy->GetThreadDelegate().BindSP(TargetClass, InMethod, Vars...); //��һ�������������߳�
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
			if (ThreadProxy->IsSuspend()) //��ȡһ����������߳�
			{
				ThreadProxy->GetThreadDelegate().BindLambda(InMethod, Vars...); //��һ�������������߳�
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
			if (ThreadProxy->IsSuspend()) //��ȡһ����������߳�
			{
				ThreadProxy->GetThreadDelegate().BindUFunction(TargetClass,InMethodName, Vars...); //��һ�������������߳�
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

/*----------------------- �����߳�(�󶨷���) ----------------------*/
#pragma region CreatedThread
	template<class UserClass, typename... VarTypes>
	FThreadHandle CreateRaw(UserClass* TargetClass, typename TMemFunPtrType<false, UserClass, void(VarTypes...)>::Type InMethod, VarTypes... Vars)
	{
		if (Pool.Num() < CPUThreadNumber)
		{
			TSharedPtr<IThreadProxy> ThreadProxy = MakeShareable(new FThreadRunnable(false));
			ThreadProxy->GetThreadDelegate().BindRaw(TargetClass, InMethod, Vars...); //˭���ð�˭�ķ�������
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

/*-------------------- ���������߳�(�󶨷���) -------------------*/
#pragma region CreatedSuspendThread

	template<class UserClass,typename... VarTypes>
	FThreadHandle CreateThreadRaw(UserClass* TargetClass, typename TMemFunPtrType<false, UserClass, void(VarTypes...)>::Type InMethod, VarTypes... Vars)
	{
		TSharedPtr<IThreadProxy> ThreadProxy = MakeShareable(new FThreadRunnable);
		ThreadProxy->GetThreadDelegate().BindRaw(TargetClass, InMethod, Vars...); //˭���ð�˭�ķ�������
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
		FScopeLock ScopeLock(&Mutex); //����
		TaskQueue.Enqueue(FSimpleDelegate::CreateRaw(TargetClass,InMethod,Var...));	//��̬�����´���
	}

	template<class UserClass, typename... VarTypes>
	FThreadHandle AddUFunctionToQueue(UserClass* TargetClass, const FName& InMethodName, VarTypes... Vars)
	{
		FScopeLock ScopeLock(&Mutex); //����
		TaskQueue.Enqueue(FSimpleDelegate::CreateUFunction(TargetClass, InMethodName, Var...));	//��̬�����´���
	}

	template<typename FuncType, typename... VarTypes>
	FThreadHandle AddLambdaToQueue(FuncType&& InMethod, VarTypes... Vars)
	{
		FScopeLock ScopeLock(&Mutex); //����
		TaskQueue.Enqueue(FSimpleDelegate::CreateLambda(InMethod, Var...));	//��̬�����´���
	}
	template<class UserClass, typename... VarTypes>
	FThreadHandle AddSPToQueue(const TSharedRef<UserClass>& TargetClass, typename TMemFunPtrType<false, UserClass, void(VarTypes...)>::Type InMethod, VarTypes... Vars)
	{
		FScopeLock ScopeLock(&Mutex); //����
		TaskQueue.Enqueue(FSimpleDelegate::CreateSP(TargetClass, InMethod, Var...));	//��̬�����´���
	}
	template<class UserClass, typename... VarTypes>
	FThreadHandle AddUObjectToQueue(UserClass* TargetClass, typename TMemFunPtrType<false, UserClass, void(VarTypes...)>::Type InMethod, VarTypes... Vars)
	{
		FScopeLock ScopeLock(&Mutex); //����
		TaskQueue.Enqueue(FSimpleDelegate::CreateUObject(TargetClass, InMethod, Var...));	//��̬�����´���
	}
protected:
	FThreadHandle UpdateThreadPool(TSharedPtr<IThreadProxy> ThreadProxy); //�����̳߳�
 
private:
	TArray<TSharedPtr<IThreadProxy>> Pool;	//�̳߳�
	TQueue<FSimpleDelegate> TaskQueue;	//������� #include "Queue"
	FCriticalSection Mutex;	//������
private:
	static TSharedPtr<FThreadManagement> ThreadManagement;
};