#pragma once
#include "CoreMinimal.h"
#include "Core/SimpleThreadType.h"

//Э�̵�����
struct SIMPLETHREAD_API FCoroutinesRequest
{
	FCoroutinesRequest(float InIntervalTime);

	//�Ƿ��������
	bool bCompleteRequest;

	//ÿһ֡��ʱ����
	float IntervalTime;
};

//Э�̽ӿڶ���
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

	//����Э��
	FORCEINLINE void Awaken() { bAwaken = true; }
protected:

	virtual void Update(FCoroutinesRequest &CoroutinesRequest) = 0;

protected:

	//Э�̶�������������洢��Э�̶���
	static TArray<TSharedPtr<ICoroutinesObject>> Array;
	uint8 bAwaken : 1;
	FSimpleThreadHandle Handle;
};
//Э�̾��
typedef TWeakPtr<ICoroutinesObject> FCoroutinesHandle;

//Э�̶���
class SIMPLETHREAD_API FCoroutinesObject :public ICoroutinesObject
{
public:
	FCoroutinesObject(const FSimpleDelegate &InSimpleDelegate);
	FCoroutinesObject(float InTotalTime,const FSimpleDelegate &InSimpleDelegate);

	//һ��ע�� ����ÿ֡����
	virtual void Update(FCoroutinesRequest &CoroutinesRequest) final;
private:

	//ע����Ҫ�Ĵ���
	FSimpleDelegate SimpleDelegate;

	//�ܹ���Ҫ�ȴ���ʱ��
	const float TotalTime;

	//��ǰʱ�䣬��Ҫ���ڼ�¼ʱ��
	float RuningTime;
};