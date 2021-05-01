#pragma once
#include "CoreMinimal.h"
#include "Interface/ThreadManageInterface.h"
#include "ManageBase/ThreadManageBase.h"
#include "Tickable.h"

//Bind		添加到任务队列中，如果有空置的线程可以直接执行该任务。
//Create	直接在线程池里找，如果有闲置线程，直接运行当前任务。
class SIMPLETHREAD_API FThreadTaskManagement : public FTreadTemplate<IThreadTaskContainer>, public FTickableGameObject
{
public:
	FThreadTaskManagement();
	~FThreadTaskManagement();

	void Init(int32 ThreadNum); //提前初始化线程(构造函数调用)

private:
	virtual void Tick(float DeltaTime); //发生在主线程 (循环 从任务队列 中取出任务给线程闲置线程)
	virtual TStatId GetStatId() const; //获取一个不明ID
};
