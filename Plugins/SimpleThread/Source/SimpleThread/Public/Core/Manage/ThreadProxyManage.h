#pragma once
#include "CoreMinimal.h"
#include "Interface/ThreadManageInterface.h"

/*------------------ 代理管理类 ------------------*/

class SIMPLETHREAD_API FThreadProxyManage : public IThreadProxyContainer 
{
public:
	~FThreadProxyManage();

	//同步(目前就一个阻塞主线程)
	bool Join(FThreadHandle Handle);

	//异步(目前就一个唤醒线程)
	bool Detach(FThreadHandle Handle);

	//获取线程状态(限制，工作，错误)
	EThreadState Joinable(FThreadHandle Handle);

	
public:
	/*----------------- 【绑定不执行】(可以执行Join Detach 来执行线程)  添加任务代理到闲置线程 --------------*/
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
	/*----------------- 【绑定执行】---------------*/
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
