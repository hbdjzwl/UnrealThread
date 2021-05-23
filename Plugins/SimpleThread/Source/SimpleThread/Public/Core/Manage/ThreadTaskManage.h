#pragma once
#include "CoreMinimal.h"
#include "Interface/ThreadManageInterface.h"
#include "ManageBase/ThreadManageBase.h"


//Bind		添加到任务队列中，如果有空置的线程可以直接执行该任务。
//Create	直接在线程池里找，如果有闲置线程，直接运行当前任务。
class SIMPLETHREAD_API FThreadTaskManagement : public FThreadTemplate<IThreadTaskContainer>
{
public:
	FThreadTaskManagement(); //调用Init创建最大数量线程
	~FThreadTaskManagement();

	void Init(int32 ThreadNum); //提前初始化线程(构造函数调用)
	void Tick(float DeltaTime); //循环 从任务队列 中取出任务给线程闲置线程
};
