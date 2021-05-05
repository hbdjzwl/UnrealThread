 #pragma once

#include "CoreMinimal.h"
#include "Core/Manage/ManageBase/ThreadManageBase.h"
#include "Interface/ThreadManageInterface.h"

//	Bind  呼叫主线程
//	Create绑定任意线程
struct SIMPLETHREAD_API FThreadGraphManage : FTreadTemplate<IGraphContainer, FGraphEventRef>
{
	//挂起
	static void Wait(const FGraphEventRef &EventRef);
	static void Wait(const FGraphEventArray &EventRef);
};