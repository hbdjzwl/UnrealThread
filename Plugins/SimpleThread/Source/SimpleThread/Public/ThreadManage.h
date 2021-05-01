//����һ���̣߳��鿴�߳��ǲ����������У��ǲ����ڽ����ˡ�
#pragma once

#include "CoreMinimal.h"
#include "Runnable/ThreadRunnableProxy.h"
#include "Core/Manage/ThreadProxyManage.h"
#include "Core/Manage/ThreadTaskManage.h"
#include "Core/Manage/ThreadAbandonableManage.h"

namespace TM
{

	//FThreadManagement ���̰߳�ȫ�ģ������׳���������
	//����ģʽ: ���̺߳������̶߳����Ե��ã�������Ҫ���м�����
	class SIMPLETHREAD_API FThreadManagement : public TSharedFromThis<FThreadManagement>
	{
	public:
		static TSharedRef<FThreadManagement> Get();
		static void Destroy();

	public:
		static FThreadProxyManage& GetProxy() { return Get()->ThreadProxtManage; }
		static FThreadTaskManagement& GetTask() { return Get()->ThreadTaskManagement; }
		static FThreadAbandonableManage& GetAbandonable() { return Get()->ThreadAbandonableManage; }


	protected:
		FThreadProxyManage ThreadProxtManage; //�Զ����̴߳������ɼ�ֱ�ӵĴ����߳�

		FThreadTaskManagement ThreadTaskManagement;	//�Զ����̳߳أ��������̳߳��ڶ���������ִ��

		FThreadAbandonableManage ThreadAbandonableManage; //ue4�̳߳���ֱ��ȡ�߳�

	private:
		static TSharedPtr<FThreadManagement> ThreadManagement;
	};
}

using namespace TM;
typedef TM::FThreadManagement GThread;