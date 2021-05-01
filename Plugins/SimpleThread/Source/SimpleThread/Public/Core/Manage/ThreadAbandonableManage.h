#include "CoreMinimal.h"
#include "Interface/ThreadManageInterface.h"
#include "ManageBase/ThreadManageBase.h"

//Bind		同步绑定 会阻塞启动线程 完成任务后激活启动线程
//Create	异步绑定 直接启动，任务完成后自动销毁
struct SIMPLETHREAD_API FThreadAbandonableManage :FThreadTemplateBase<IAbandonableContainer>
{

};