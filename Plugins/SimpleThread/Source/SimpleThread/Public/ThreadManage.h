//创建一个线程，查看线程是不是在运行中，是不是在结束了。
#pragma once

#include "CoreMinimal.h"
#include "Runnable/ThreadRunnableProxy.h"
#include "Core/Manage/ThreadProxyManage.h"
#include "Core/Manage/ThreadTaskManage.h"
#include "Core/Manage/ThreadAbandonableManage.h"
#include "Core/Manage/CoroutinesManage.h"
#include "Core/Manage/ThreadGraphManage.h"
#include "Core/Manage/ResourceLoadingManage.h"
#include "Tickable.h"




namespace TM
{

	//FThreadManagement 是线程安全的，不容易出现死锁。
	//单例模式: 主线程和其它线程都可以调用，所用需要进行加锁。
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
		static FThreadGraphManage& GetGraph() { return Get()->ThreadGrapManage; }
		static FResourceLoadingManage& GetResourceLoading() { return Get()->ResourceLoadingManage; }
	protected:
		FThreadProxyManage ThreadProxtManage;				//自定义线程创建，可简单直接的创建线程
		FThreadTaskManagement ThreadTaskManagement;			//自定义线程池，可以往线程池内丢任务，令其执行
		FThreadAbandonableManage ThreadAbandonableManage;	//ue4线程池内直接取线程
		FCoroutinesManage CoroutinesManage;					//协程
		FThreadGraphManage ThreadGrapManage;				//可以设置前置任务的UE4线程池
		FResourceLoadingManage ResourceLoadingManage;		//资源读取
	private:
		static TSharedPtr<FThreadManagement> ThreadManagement; //单例对象
	};
}

using namespace TM;
typedef TM::FThreadManagement GThread;

void Examle()
{
	TArray<FSoftObjectPath> ObjectPath;
	TSharedPtr<FStreamableHandle> Handle;

	auto La = [&Handle]()
	{
		TArray<UObject*> ExampleObject;
		Handle->GetLoadedAssets(ExampleObject);
	};

	//异步使用方法
	Handle = GThread::GetResourceLoading() >> ObjectPath >> FSimpleDelegate::CreateLambda(La);

	//同步
	Handle = GThread::GetResourceLoading() << ObjectPath;
}
