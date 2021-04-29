#pragma once
#include "CoreMinimal.h"


//自动回收线程
struct SIMPLETHREAD_API FSimpleAbandonable : public FNonAbandonableTask
{


	FSimpleAbandonable(const FSimpleDelegate& InThreadDelegate);

	void DoWork();

	FORCEINLINE TStatId GetStatId() const
	{
		RETURN_QUICK_DECLARE_CYCLE_STAT(FSimpleAbandonable, STATGROUP_ThreadPoolAsyncTasks);
	}
protected:
	FSimpleDelegate ThreadDelegate;
};