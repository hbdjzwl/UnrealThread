#pragma once
#include "CoreMinimal.h"
#include "Interface/ThreadManageInterface.h"
#include "ManageBase/ThreadManageBase.h"
#include "Tickable.h"

//Bind		��ӵ���������У�����п��õ��߳̿���ֱ��ִ�и�����
//Create	ֱ�����̳߳����ң�����������̣߳�ֱ�����е�ǰ����
class SIMPLETHREAD_API FThreadTaskManagement : public FTreadTemplate<IThreadTaskContainer>, public FTickableGameObject
{
public:
	FThreadTaskManagement();
	~FThreadTaskManagement();

	void Init(int32 ThreadNum); //��ǰ��ʼ���߳�(���캯������)

private:
	virtual void Tick(float DeltaTime); //���������߳� (ѭ�� ��������� ��ȡ��������߳������߳�)
	virtual TStatId GetStatId() const; //��ȡһ������ID
};
