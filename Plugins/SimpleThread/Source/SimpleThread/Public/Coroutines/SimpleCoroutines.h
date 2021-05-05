#pragma once
#include "CoreMinimal.h"
#include "Core/SimpleThreadType.h"

//协程的请求
struct SIMPLETHREAD_API FCoroutinesRequest
{
	FCoroutinesRequest(float InIntervalTime);

	//是否完成请求
	bool bCompleteRequest;

	//每一帧的时间间隔
	float IntervalTime;
};

//协程接口对象
class SIMPLETHREAD_API ICoroutinesObject :public TSharedFromThis<ICoroutinesObject>
{
	friend class ICoroutinesContainer;
public:
	ICoroutinesObject();
	virtual ~ICoroutinesObject(){}

	bool operator==(const ICoroutinesObject& SimpleThreadHandle)
	{
		return this->Handle == SimpleThreadHandle.Handle;
	}

	//唤醒协程
	FORCEINLINE void Awaken() { bAwaken = true; }
protected:

	virtual void Update(FCoroutinesRequest &CoroutinesRequest) = 0;

protected:

	//协程对象容器，里面存储着协程对象
	static TArray<TSharedPtr<ICoroutinesObject>> Array;
	uint8 bAwaken : 1;
	FSimpleThreadHandle Handle;
};
//协程句柄
typedef TWeakPtr<ICoroutinesObject> FCoroutinesHandle;

//协程对象
class SIMPLETHREAD_API FCoroutinesObject :public ICoroutinesObject
{
public:
	FCoroutinesObject(const FSimpleDelegate &InSimpleDelegate);
	FCoroutinesObject(float InTotalTime,const FSimpleDelegate &InSimpleDelegate);

	//一旦注册 将会每帧更新
	virtual void Update(FCoroutinesRequest &CoroutinesRequest) final;
private:

	//注册需要的代理
	FSimpleDelegate SimpleDelegate;

	//总共需要等待的时间
	const float TotalTime;

	//当前时间，主要用于记录时间
	float RuningTime;
};