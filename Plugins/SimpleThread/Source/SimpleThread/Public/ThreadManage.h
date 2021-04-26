//����һ���̣߳��鿴�߳��ǲ����������У��ǲ����ڽ����ˡ�
#pragma once

#include "CoreMinimal.h"
#include "Runnable/ThreadRunnableProxy.h"
#include "Core/Manage/ThreadProxyManage.h"
#include "Core/Manage/ThreadTaskManage.h"

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
		FORCEINLINE FThreadProxyManage& GetProxy() { return ThreadProxtManage; }
		FORCEINLINE FThreadTaskManagement& GetTask() { return ThreadTaskManagement; }

	protected:
		FThreadProxyManage ThreadProxtManage;
		FThreadTaskManagement ThreadTaskManagement;

	private:
		static TSharedPtr<FThreadManagement> ThreadManagement;
	};
}

using namespace TM;
typedef TM::FThreadManagement GThread;