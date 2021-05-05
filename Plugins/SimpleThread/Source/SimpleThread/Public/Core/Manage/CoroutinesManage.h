#pragma once
#include "CoreMinimal.h"
#include "Interface/ThreadManageInterface.h"
#include "Core/Manage/ManageBase/ThreadManageBase.h"

//Bind		绑定后可以设置时间，多久执行
//Create	绑定完毕返回一个Handle ,由程序员来决定什么时候执行
struct SIMPLETHREAD_API FCoroutinesManage :FThreadTemplateBase<ICoroutinesContainer, FCoroutinesHandle>
{
public:
	FCoroutinesManage();

	//用于监视任务情况
	void Tick(float );
public:
	template<class UserClass, typename... VarTypes>
	void BindRaw(float InTotalTime, UserClass *TargetClass, typename TMemFunPtrType<false, UserClass, void(VarTypes...)>::Type InMethod, VarTypes... Vars)
	{
		*this << InTotalTime << FSimpleDelegate::CreateRaw(TargetClass, InMethod, Vars...);
	}

	template<class UserClass, typename... VarTypes>
	void BindUObject(float InTotalTime, UserClass *TargetClass, typename TMemFunPtrType<false, UserClass, void(VarTypes...)>::Type InMethod, VarTypes... Vars)
	{
		*this << InTotalTime << FSimpleDelegate::CreateUObject(TargetClass, InMethod, Vars...);
	}

	template<class UserClass, typename... VarTypes>
	void BindSP(float InTotalTime, const TSharedRef<UserClass>& TargetClass, typename TMemFunPtrType<false, UserClass, void(VarTypes...)>::Type InMethod, VarTypes... Vars)
	{
		*this << InTotalTime << FSimpleDelegate::CreateSP(TargetClass, InMethod, Vars...);
	}

	template<typename FunctorType, typename... VarTypes>
	void BindLambda(float InTotalTime, FunctorType &&InMethod, VarTypes... Vars)
	{
		*this << InTotalTime << FSimpleDelegate::CreateLambda(InMethod, Vars...);
	}

	template<class UserClass, typename... VarTypes>
	void BindUFunction(float InTotalTime, UserClass *TargetClass, const FName& InMethod, VarTypes... Vars)
	{
		*this << InTotalTime << FSimpleDelegate::CreateUFunction(TargetClass, InMethod, Vars...);
	}
};


