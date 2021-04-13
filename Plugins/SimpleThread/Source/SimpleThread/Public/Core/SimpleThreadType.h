#pragma once
#include "CoreMinimal.h"

/* ---- ͨ���߳�Handle���鿴�߳�״̬ ---*/
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
	/*--- ��ֹ�����ظ���ID��Ѱ���߳� ---*/
	uint64 GUIDTypeA;
	uint64 GUIDTypeB;
	uint64 GUIDTypeC;
	uint64 GUIDTypeD;
};

typedef TWeakPtr<FSimpleThreadHandle> FThreadHandle;
typedef TFunction<void()> FThreadLambda;
