#pragma once
#include "CoreMinimal.h"

/* ---- 通过线程Handle来查看线程状态 ---*/
struct SIMPLETHREAD_API FSimpleThreadHandle : public TSharedFromThis<FSimpleThreadHandle>
{
	FSimpleThreadHandle();

	bool operator==(const FSimpleThreadHandle& SimpleThreadHandle)
	{
		return
			this->GUIDTypeA == SimpleThreadHandle.GUIDTypeA &&
			this->GUIDTypeB == SimpleThreadHandle.GUIDTypeB &&
			this->GUIDTypeC == SimpleThreadHandle.GUIDTypeC &&
			this->GUIDTypeD == SimpleThreadHandle.GUIDTypeD;
	}
protected:
	/*--- 防止出现重复的ID，寻找线程 ---*/
	uint64 GUIDTypeA;
	uint64 GUIDTypeB;
	uint64 GUIDTypeC;
	uint64 GUIDTypeD;
};

typedef TWeakPtr<FSimpleThreadHandle> FThreadHandle;
typedef TFunction<void()> FThreadLambda;
