#pragma once
#include "CoreMinimal.h"
#include "Interface/ThreadManageInterface.h"

class SIMPLETHREAD_API FThreadProxyManage : public IThreadProxyContainer 
{
public:
	~FThreadProxyManage();

	//ͬ��
	bool Join(FThreadHandle Handle);

	//�첽
	bool Detach(FThreadHandle Handle);

	//�ж�ָ��Handle�Ƿ����ͬ��
	EThreadState Joinable(FThreadHandle Handle);

	
public:
	//�� ���� ��ִ�� ����ִ��Join Detach ��ִ���߳�
	template<class UserClass, typename... VarTypes>
	FThreadHandle BindRaw(UserClass* TargetClass, typename TMemFunPtrType<false, UserClass, void(VarTypes...)>::Type InMethod, VarTypes... Vars)
	{
		return *this << FSimpleDelegate::BindRaw(TargetClass, InMethod, Vars...);
	}

	template<class UserClass, typename... VarTypes>
	FThreadHandle BindUFunction(UserClass* TargetClass, const FName& InMethodName, VarTypes... Vars)
	{
		return *this << FSimpleDelegate::BindUFunction(TargetClass, InMInMethodNameethod, Vars...);
	}

	template<typename FuncType, typename... VarTypes>
	FThreadHandle BindLambda(FuncType&& InMethod, VarTypes... Vars)
	{
		return *this << FSimpleDelegate::BindLambda(InMethod, Vars...);
	}

	template<class UserClass, typename... VarTypes>
	FThreadHandle BindSP(const TSharedRef<UserClass>& TargetClass, typename TMemFunPtrType<false, UserClass, void(VarTypes...)>::Type InMethod, VarTypes... Vars)
	{
		return *this << FSimpleDelegate::BindSP(TargetClass, InMethod, Vars...);
	}

	template<class UserClass, typename... VarTypes>
	FThreadHandle BindUObject(UserClass* TargetClass, typename TMemFunPtrType<false, UserClass, void(VarTypes...)>::Type InMethod, VarTypes... Vars)
	{
		return *this << FSimpleDelegate::BindUObject(TargetClass, InMethod, Vars...);
	}

public:
	//������ֱ��ִ��
	template<class UserClass, typename... VarTypes>
	FThreadHandle CreateRaw(UserClass* TargetClass, typename TMemFunPtrType<false, UserClass, void(VarTypes...)>::Type InMethod, VarTypes... Vars)
	{
		return *this ^ FSimpleDelegate::BindRaw(TargetClass, InMethod, Vars...);
	}
	template<class UserClass, typename... VarTypes>
	FThreadHandle CreateUFunction(UserClass* TargetClass, const FName& InMethodName, VarTypes... Vars)
	{
		return *this ^ FSimpleDelegate::BindUFunction(TargetClass, InMethodName, Vars...);
	}

	template<typename FuncType, typename... VarTypes>
	FThreadHandle CreateLambda(FuncType&& InMethod, VarTypes... Vars)
	{
		return *this ^ FSimpleDelegate::BindLambda(InMethod, Vars...);
	}

	template<class UserClass, typename... VarTypes>
	FThreadHandle CtreateSP(const TSharedRef<UserClass>& TargetClass, typename TMemFunPtrType<false, UserClass, void(VarTypes...)>::Type InMethod, VarTypes... Vars)
	{
		return *this ^ FSimpleDelegate::BindSP(TargetClass, InMethod, Vars...);
	}

	template<class UserClass, typename... VarTypes>
	FThreadHandle CtreateUObject(UserClass* TargetClass, typename TMemFunPtrType<false, UserClass, void(VarTypes...)>::Type InMethod, VarTypes... Vars)
	{
		return *this ^ FSimpleDelegate::BindUObject(TargetClass, InMethod, Vars...);
	}
};