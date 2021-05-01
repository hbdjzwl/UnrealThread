//创建一个线程，查看线程是不是在运行中，是不是在结束了。
#pragma once

#include "CoreMinimal.h"
#include "Runnable/ThreadRunnableProxy.h"
#include "Core/Manage/ThreadProxyManage.h"
#include "Core/Manage/ThreadTaskManage.h"
#include "Core/Manage/ThreadAbandonableManage.h"

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
		static FThreadProxyManage& GetProxy() { return Get()->ThreadProxtManage; }
		static FThreadTaskManagement& GetTask() { return Get()->ThreadTaskManagement; }
		static FThreadAbandonableManage& GetAbandonable() { return Get()->ThreadAbandonableManage; }


	protected:
		FThreadProxyManage ThreadProxtManage; //自定义线程创建，可简单直接的创建线程

		FThreadTaskManagement ThreadTaskManagement;	//自定义线程池，可以往线程池内丢任务，令其执行

		FThreadAbandonableManage ThreadAbandonableManage; //ue4线程池内直接取线程

	private:
		static TSharedPtr<FThreadManagement> ThreadManagement;
	};
}

using namespace TM;
typedef TM::FThreadManagement GThread;