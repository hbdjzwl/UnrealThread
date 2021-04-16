//����һ���̣߳��鿴�߳��ǲ����������У��ǲ����ڽ����ˡ�
#pragma once

#include "CoreMinimal.h"
#include "Runnable/ThreadRunnableProxy.h"

enum class EThreadState
{
	LEISURELY,
	WORKING,
	ERROR
};



//��������
// ���̺߳������̶߳����Ե��ã�������Ҫ���м�����
class SIMPLETHREAD_API FThreadManagement : public TSharedFromThis<FThreadManagement>
{
public:
	static TSharedRef<FThreadManagement> Get();
	static void Destroy();


public:
	//�жϵ�ǰ�߳��Ƿ�������
	EThreadState ProceduralProgress(FThreadHandle Handle);
	bool Do(FThreadHandle Handle); //ִ���Ѿ��󶨺õ��̣߳�����߳�û�󶨺õ���Do()����fasle;
private:
	void CleanAllThread();	//���ɾ�������߳�
	void CleanThread(FThreadHandle Handle);	//���ָ���߳�

public:
/*-------------------- �󶨷��� -------------------*/
#pragma region DelegateBind
	/*------- BinRaw --------*/
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
	/*------- BinUObject --------*/
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
	/*------- BindSP --------*/
	template<class UserClass, typename... VarTypes>
	FThreadHandle BindSP(UserClass* TargetClass, typename TMemFunPtrType<false, UserClass, void(VarTypes...)>::Type InMethod, VarTypes... Vars)
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
	/*------- BindLambda --------*/
	template<class UserClass, typename... VarTypes>
	FThreadHandle BindLambda(TFunction<void(VarTypes...)> InMethod, VarTypes... Vars)
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

	/*------- BinUFunction --------*/
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


/*-------------------- �����߳� + �󶨷��� -------------------*/
#pragma region DelegateCreatedAndBind
	/*------- BinRaw --------*/
	template<class UserClass,typename... VarTypes>
	FThreadHandle CreateThreadRaw(UserClass* TargetClass, typename TMemFunPtrType<false, UserClass, void(VarTypes...)>::Type InMethod, VarTypes... Vars)
	{
		TSharedPtr<IThreadProxy> ThreadProxy = MakeShareable(new FThreadRunnable);
		ThreadProxy->GetThreadDelegate().BindRaw(TargetClass, InMethod, Vars...); //˭���ð�˭�ķ�������
		return UpdateThreadPool(ThreadProxy);
	}

	/*------- BinUFunction --------*/
	template<class UserClass,typename... VarTypes>
	FThreadHandle CreateThreadUFunction(UserClass* TargetClass, const FName& InMethodName, VarTypes... Vars)
	{
		TSharedPtr<IThreadProxy> ThreadProxy = MakeShareable(new FThreadRunnable);
		ThreadProxy->GetThreadDelegate().BindUFunction(TargetClass,InMethodName,Vars...);
		return UpdateThreadPool(ThreadProxy);
	}

	/*------- BinLambda --------*/
	template<typename... VarTypes>
	FThreadHandle CreateThreadLambda(TFunction<void(VarTypes...)> InMethod, VarTypes... Vars)
	{
		TSharedPtr<IThreadProxy> ThreadProxy = MakeShareable(new FThreadRunnable);
		ThreadProxy->GetThreadDelegate().BindLambda(InMetod,Vars...);
		return UpdateThreadPool(ThreadProxy);
	}

	/*------- BinSP --------*/
	template<class UserClass, typename... VarTypes>
	FThreadHandle CtreateThreadSP(UserClass* TargetClass, typename TMemFunPtrType<false, UserClass, void(VarTypes...)>::Type InMethod, VarTypes... Vars)
	{
		TSharedPtr<IThreadProxy> ThreadProxy = MakeShareable(new FThreadRunnable);
		ThreadProxy->GetThreadDelegate().BindSP(TargetClass, InMethod, Vars...);
		return UpdateThreadPool(ThreadProxy);
	}

	/*------- BinObject --------*/
	template<class UserClass, typename... VarTypes>
	FThreadHandle CtreateThreadUObject(UserClass* TargetClass, typename TMemFunPtrType<false, UserClass, void(VarTypes...)>::Type InMethod, VarTypes... Vars)
	{
		TSharedPtr<IThreadProxy> ThreadProxy = MakeShareable(new FThreadRunnable);
		ThreadProxy->GetThreadDelegate().BindUObject(TargetClass, InMethod, Vars...);
		return UpdateThreadPool(ThreadProxy);
	}
#pragma endregion
	FThreadHandle CreateThread(const FThreadLambda& ThreadLambda);

protected:
	FThreadHandle UpdateThreadPool(TSharedPtr<IThreadProxy> ThreadProxy); //�����̳߳�

	 
private:
	//�̳߳�
	TArray<TSharedPtr<IThreadProxy>> Pool; 

private:
	static TSharedPtr<FThreadManagement> ThreadManagement;
};