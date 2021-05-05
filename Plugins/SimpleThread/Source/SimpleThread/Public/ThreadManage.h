//����һ���̣߳��鿴�߳��ǲ����������У��ǲ����ڽ����ˡ�
#pragma once

#include "CoreMinimal.h"
#include "Runnable/ThreadRunnableProxy.h"
#include "Core/Manage/ThreadProxyManage.h"
#include "Core/Manage/ThreadTaskManage.h"
#include "Core/Manage/ThreadAbandonableManage.h"
#include "Core/Manage/CoroutinesManage.h"
#include "Core/Manage/ThreadGraphManage.h"
#include "Tickable.h"


namespace TM
{

	//FThreadManagement ���̰߳�ȫ�ģ������׳���������
	//����ģʽ: ���̺߳������̶߳����Ե��ã�������Ҫ���м�����
	class SIMPLETHREAD_API FThreadManagement : public TSharedFromThis<FThreadManagement>,public FTickableGameObject
	{
	public:
		static TSharedRef<FThreadManagement> Get();
		static void Destroy();

	private:
		virtual void Tick(float DeltaTime) override;
		virtual TStatId GetStatId()const override;

	public:
		static FThreadProxyManage& GetProxy() { return Get()->ThreadProxtManage; }
		static FThreadTaskManagement& GetTask() { return Get()->ThreadTaskManagement; }
		static FThreadAbandonableManage& GetAbandonable() { return Get()->ThreadAbandonableManage; }
		static FCoroutinesManage& GetCoroutines() { return Get()->CoroutinesManage; }

	protected:
		FThreadProxyManage ThreadProxtManage;				//�Զ����̴߳������ɼ�ֱ�ӵĴ����߳�
		FThreadTaskManagement ThreadTaskManagement;			//�Զ����̳߳أ��������̳߳��ڶ���������ִ��
		FThreadAbandonableManage ThreadAbandonableManage;	//ue4�̳߳���ֱ��ȡ�߳�
		FCoroutinesManage CoroutinesManage;					//Э��
		FThreadGraphManage ThreadGrapManage;				//��������ǰ�������UE4�̳߳�
	private:
		static TSharedPtr<FThreadManagement> ThreadManagement; //��������
	};
}

using namespace TM;
typedef TM::FThreadManagement GThread;