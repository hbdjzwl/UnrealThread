#pragma once
#include "CoreMinimal.h"
#include "Interface/ThreadManageInterface.h"
#include "ManageBase/ThreadManageBase.h"

/*------------------ 代理管理类 ------------------*/
//Bind		创建线程并且绑定任务但不执行，通过Join与Detach来决定异步执行还是同步执行
//Create	创建线程并且直接执行任务，是最快捷的异步方式

class SIMPLETHREAD_API FThreadProxyManage : public FThreadTemplate<IThreadProxyContainer,FThreadHandle>
{
public:
	~FThreadProxyManage();

	//同步(目前就一个阻塞主线程)
	bool Join(FThreadHandle Handle);

	//异步(目前就一个唤醒线程)
	bool Detach(FThreadHandle Handle);

	//获取线程状态(限制，工作，错误)
	EThreadState Joinable(FThreadHandle Handle);

	
};
