#pragma once
//下面的网址讲解了如何使用本套插件SimpleThread
//DocURL：			https://zhuanlan.zhihu.com/p/78023533
//
//如果想学习当前线程插件的详细开发可以参考这个网址
//MarketplaceURL : http://www.aboutcg.org/course/tut_ue4suite_190813/?action=
//
//如果想了解更多关于UE4教程请参考：
//URL : https://zhuanlan.zhihu.com/p/60117613

#include "CoreMinimal.h"

//			<<
//			>> 

//提供基础模板类，便于匹配
template<class FTask, typename ReturnType = void>
class FThreadTemplateBase :public FTask
{
public:
	FThreadTemplateBase():FTask(){}

	template<class UserClass, typename... VarTypes>
	ReturnType CreateRaw(UserClass *TargetClass, typename TMemFunPtrType<false, UserClass, void(VarTypes...)>::Type InMethod, VarTypes... Vars)
	{
		return *this >> FSimpleDelegate::CreateRaw(TargetClass, InMethod, Vars...);
	}

	template<class UserClass, typename... VarTypes>
	ReturnType CreateUObject(UserClass *TargetClass, typename TMemFunPtrType<false, UserClass, void(VarTypes...)>::Type InMethod, VarTypes... Vars)
	{
		return *this >> FSimpleDelegate::CreateUObject(TargetClass, InMethod, Vars...);
	}

	template<class UserClass, typename... VarTypes>
	ReturnType CreateSP(const TSharedRef<UserClass>& TargetClass, typename TMemFunPtrType<false, UserClass, void(VarTypes...)>::Type InMethod, VarTypes... Vars)
	{
		return *this >> FSimpleDelegate::CreateSP(TargetClass, InMethod, Vars...);
	}

	template<typename FunctorType, typename... VarTypes>
	ReturnType CreateLambda(FunctorType &&InMethod, VarTypes... Vars)
	{
		return *this >> FSimpleDelegate::CreateLambda(InMethod, Vars...);
	}

	template<class UserClass, typename... VarTypes>
	ReturnType CreateUFunction(UserClass *TargetClass, const FName& InMethod, VarTypes... Vars)
	{
		return *this >> FSimpleDelegate::CreateUFunction(TargetClass, InMethod, Vars...);
	}
};

//提供基础模板类，便于匹配
template<class FTask, typename ReturnType = void>
class FTreadTemplate :public FThreadTemplateBase<FTask, ReturnType>
{
	//Bind 
public:
	FTreadTemplate():FThreadTemplateBase<FTask, ReturnType>(){}

	template<class UserClass, typename... VarTypes>
	ReturnType BindRaw(UserClass *TargetClass, typename TMemFunPtrType<false, UserClass, void(VarTypes...)>::Type InMethod, VarTypes... Vars)
	{
		return *this << FSimpleDelegate::CreateRaw(TargetClass, InMethod, Vars...);
	}

	template<class UserClass, typename... VarTypes>
	ReturnType BindUObject(UserClass *TargetClass, typename TMemFunPtrType<false, UserClass, void(VarTypes...)>::Type InMethod, VarTypes... Vars)
	{
		return *this << FSimpleDelegate::CreateUObject(TargetClass, InMethod, Vars...);
	}

	template<class UserClass, typename... VarTypes>
	ReturnType BindSP(const TSharedRef<UserClass>& TargetClass, typename TMemFunPtrType<false, UserClass, void(VarTypes...)>::Type InMethod, VarTypes... Vars)
	{
		return *this << FSimpleDelegate::CreateSP(TargetClass, InMethod, Vars...);
	}

	template<typename FunctorType, typename... VarTypes>
	ReturnType BindLambda(FunctorType &&InMethod, VarTypes... Vars)
	{
		return *this << FSimpleDelegate::CreateLambda(InMethod, Vars...);
	}

	template<class UserClass, typename... VarTypes>
	ReturnType BindUFunction(UserClass *TargetClass, const FName& InMethod, VarTypes... Vars)
	{
		return *this << FSimpleDelegate::CreateUFunction(TargetClass, InMethod, Vars...);
	}
};
