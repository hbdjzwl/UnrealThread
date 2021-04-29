#pragma once
#include "CoreMinimal.h"
#include "Interface/ThreadManageInterface.h"
#include "Tickable.h"

class SIMPLETHREAD_API FThreadTaskManagement : public IThreadTaskContainer, public FTickableGameObject
{
public:
	FThreadTaskManagement();
	~FThreadTaskManagement();

	void Init(int32 ThreadNum); //提前初始化线程(构造函数调用)

public:
	//【绑定不执行】 可以执行Join Detach 来执行线程  ， 寻找限制线程添加任务
	template<class UserClass, typename... VarTypes>
	FThreadHandle CreateRaw(UserClass* TargetClass, typename TMemFunPtrType<false, UserClass, void(VarTypes...)>::Type InMethod, VarTypes... Vars)
	{
		return *this^FSimpleDelegate::CreateRaw(TargetClass, InMethod, Vars...);
	}

	template<class UserClass, typename... VarTypes>
	FThreadHandle CreateUFunction(UserClass* TargetClass, const FName& InMethodName, VarTypes... Vars)
	{
		return *this ^ FSimpleDelegate::CreateUFunction(TargetClass, InMethodName, Vars...);
	}

	template<typename FuncType, typename... VarTypes>
	FThreadHandle CreateLambda(FuncType&& InMethod, VarTypes... Vars)
	{
		return *this ^ FSimpleDelegate::CreateLambda(InMethod, Vars...);
	}

	template<class UserClass, typename... VarTypes>
	FThreadHandle CtreateSP(const TSharedRef<UserClass>& TargetClass, typename TMemFunPtrType<false, UserClass, void(VarTypes...)>::Type InMethod, VarTypes... Vars)
	{
		return *this ^ FSimpleDelegate::CreateSP(TargetClass, InMethod, Vars...);
	}

	template<class UserClass, typename... VarTypes>
	FThreadHandle CtreateUObject(UserClass* TargetClass, typename TMemFunPtrType<false, UserClass, void(VarTypes...)>::Type InMethod, VarTypes... Vars)
	{
		return *this ^ FSimpleDelegate::CreateUObject(TargetClass, InMethod, Vars...);
	}

private:
	virtual void Tick(float DeltaTime); //发生在主线程 (循环 从任务队列 中取出任务给线程闲置线程)
	virtual TStatId GetStatId() const; //获取一个不明ID
};
