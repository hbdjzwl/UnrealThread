//����һ���̣߳��鿴�߳��ǲ����������У��ǲ����ڽ����ˡ�
#pragma once

#include "CoreMinimal.h"
#include "Runnable/ThreadRunnableProxy.h"
//��������
// ���̺߳������̶߳����Ե��ã�������Ҫ���м�����
class SIMPLETHREAD_API FThreadManagement : public TSharedFromThis<FThreadManagement>
{
public:
	static TSharedRef<FThreadManagement> Get();
	static void Destroy();


public:
	//�жϵ�ǰ�߳��Ƿ�������
	bool ProceduralProgress(FThreadHandle Handle);	
	bool Do(FThreadHandle Handle); //ִ���Ѿ��󶨺õ��̣߳�����߳�û�󶨺õ���Do()����fasle;
private:
	void CleanAllThread();	//���ɾ�������߳�
	void CleanThread(FThreadHandle Handle);	//���ָ���߳�

public:
/*-------------------- �󶨷��� -------------------*/
	/*------- BinRaw --------*/
	template<class UserClass, typename... VarTypes>
	FThreadHandle BindRaw(UserClass* TargetClass, typename TMemFunPtrType<false, UserClass, void(VarTypes...)>::Type InMethod, VarTypes... Vars)
	{
		FThreadHandle Handle;
		for (auto& ThreadProxy : Poll)
		{
			if (ProceduralProgress(ThreadProxy->GetThreadHandle())) //��ȡһ�������߳�
			{
				ThreadProxy->GetThreadDelegate().BindRaw(TargetClass, InMethod, Vars...); //��һ�������������߳�
				Handle = ThreadProxy->GetThreadHandle();
				break;
			}
		}
		if (!Handle.IsValid())
		{
			CreateThreadRaw<UserClass,VarTypes...>(TargetClass, InMethod, Vars...);
		}
		return Handle;
	}

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
	FThreadHandle CreateThreadFunction(UserClass* TargetClass, const FName& InMethodName, VarTypes... Vars)
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