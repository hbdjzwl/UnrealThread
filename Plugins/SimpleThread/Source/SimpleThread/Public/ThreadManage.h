//创建一个线程，查看线程是不是在运行中，是不是在结束了。
#pragma once

#include "CoreMinimal.h"
#include "Runnable/ThreadRunnableProxy.h"
#include "Core/Manage/ThreadProxyManage.h"
#include "Core/Manage/ThreadTaskManage.h"

namespace TM
{

	//FThreadManagement 是线程安全的，不容易出现死锁。
	//单例模式: 主线程和其它线程都可以调用，所用需要进行加锁。
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