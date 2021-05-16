#pragma once
#include "CoreMinimal.h"
#include "Interface/ThreadManageInterface.h"
#include "ManageBase/ThreadManageBase.h"

/*------------------ ��������� ------------------*/
//Bind		�����̲߳��Ұ����񵫲�ִ�У�ͨ��Join��Detach�������첽ִ�л���ͬ��ִ��
//Create	�����̲߳���ֱ��ִ�����������ݵ��첽��ʽ

class SIMPLETHREAD_API FThreadProxyManage : public FThreadTemplate<IThreadProxyContainer,FThreadHandle>
{
public:
	~FThreadProxyManage();

	//ͬ��(Ŀǰ��һ���������߳�)
	bool Join(FThreadHandle Handle);

	//�첽(Ŀǰ��һ�������߳�)
	bool Detach(FThreadHandle Handle);

	//��ȡ�߳�״̬(���ƣ�����������)
	EThreadState Joinable(FThreadHandle Handle);

	
};
