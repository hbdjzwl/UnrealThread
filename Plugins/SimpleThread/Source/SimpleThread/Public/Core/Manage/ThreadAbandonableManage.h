#include "CoreMinimal.h"
#include "Interface/ThreadManageInterface.h"
#include "ManageBase/ThreadManageBase.h"

//Bind		ͬ���� �����������߳� �������󼤻������߳�
//Create	�첽�� ֱ��������������ɺ��Զ�����
struct SIMPLETHREAD_API FThreadAbandonableManage :FThreadTemplate<IAbandonableContainer>
{

};