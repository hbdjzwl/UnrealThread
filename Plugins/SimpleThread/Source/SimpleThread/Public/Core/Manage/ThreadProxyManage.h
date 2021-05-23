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

	//ͬ��(�����̲߳��������߳�)
	bool Join(FThreadHandle Handle);

	//�첽(�����߳�)
	bool Detach(FThreadHandle Handle);

	//��ȡָ���߳�״̬(���ƣ�����������)
	EThreadState Joinable(FThreadHandle Handle);

	
};
