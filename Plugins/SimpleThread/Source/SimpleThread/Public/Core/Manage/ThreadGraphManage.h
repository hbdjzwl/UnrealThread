 #pragma once

#include "CoreMinimal.h"
#include "Core/Manage/ManageBase/ThreadManageBase.h"
#include "Interface/ThreadManageInterface.h"

//	Bind  �������߳�
//	Create�������߳�
struct SIMPLETHREAD_API FThreadGraphManage : FTreadTemplate<IGraphContainer, FGraphEventRef>
{
	//����
	static void Wait(const FGraphEventRef &EventRef);
	static void Wait(const FGraphEventArray &EventRef);
};