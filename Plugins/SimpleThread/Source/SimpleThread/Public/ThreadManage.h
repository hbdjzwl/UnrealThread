//����һ���̣߳��鿴�߳��ǲ����������У��ǲ����ڽ����ˡ�
//
#pragma once

#include "CoreMinimal.h"
#include "Runnable/ThreadRunnableProxy.h"
//����
class SIMPLETHREAD_API FThreadManagement : public TSharedFromThis<FThreadManagement>
{
public:
	static TSharedRef<FThreadManagement> Get();
	static void Destroy();


public:
	template<class UserClass,typename... VarTypes>
	FThreadHandle CreateThreadRaw(UserClass* TargetClass, typename TMemFunPtrType<false, UserClass, void(VarTypes...)>::Type InMethod, VarTypes... Vars)
	{
		TSharedPtr<IThreadProxy> ThreadProxy = MakeShareable(new FThreadRunnable);
		ThreadProxy->GetThreadDelegate().BindRaw(TargetClass, InMethod, Vars...); //˭���ð�˭�ķ�������
		return UpdateThreadPool(ThreadProxy);
	}

	FThreadHandle CreateThread(const FThreadLambda& ThreadLambda);

protected:
	FThreadHandle UpdateThreadPool(TSharedPtr<IThreadProxy> ThreadProxy); //�����̳߳�

	 
private:
	//�̳߳�
	TArray<TSharedPtr<IThreadProxy>> Pool; 

private:
	static TSharedPtr<FThreadManagement> ThreadManagement;
};