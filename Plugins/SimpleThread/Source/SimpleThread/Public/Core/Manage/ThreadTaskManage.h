#pragma once
#include "CoreMinimal.h"
#include "Interface/ThreadManageInterface.h"
#include "ManageBase/ThreadManageBase.h"


//Bind		��ӵ���������У�����п��õ��߳̿���ֱ��ִ�и�����
//Create	ֱ�����̳߳����ң�����������̣߳�ֱ�����е�ǰ����
class SIMPLETHREAD_API FThreadTaskManagement : public FThreadTemplate<IThreadTaskContainer>
{
public:
	FThreadTaskManagement(); //����Init������������߳�
	~FThreadTaskManagement();

	void Init(int32 ThreadNum); //��ǰ��ʼ���߳�(���캯������)
	void Tick(float DeltaTime); //ѭ�� ��������� ��ȡ��������߳������߳�
};
