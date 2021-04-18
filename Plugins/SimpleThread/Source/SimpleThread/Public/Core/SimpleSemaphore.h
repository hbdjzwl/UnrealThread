#pragma once
#include "CoreMinimal.h"

//���ź���
struct FSimpleSemaphore
{
	FSimpleSemaphore();
	~FSimpleSemaphore();
	void Wait();
	void Wait(uint32 WaitTime,const bool bIgnoreThreadIdleStats = false);
	void Trigger();
	FORCEINLINE bool IsWait() { return bWait; }
private:
	FEvent* Event; //�ź�������
	uint8 bWait : 1; //���ֽ�λ�����ڲ���
};