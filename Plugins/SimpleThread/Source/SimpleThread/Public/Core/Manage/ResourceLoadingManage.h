#pragma once
#include "CoreMinimal.h"
#include "Interface/ThreadManageInterface.h"
#include "Core/Manage/ManageBase/ThreadManageBase.h"

//Create ���԰��첽������֪ͨ���¼� //>> 
struct SIMPLETHREAD_API FResourceLoadingManage :FThreadTemplateBase<IStreamableContainer,TSharedPtr<struct FStreamableHandle>>
{
	typedef FThreadTemplateBase<IStreamableContainer,TSharedPtr<struct FStreamableHandle>> Super;

	FResourceLoadingManage();
	~FResourceLoadingManage();

protected:

	//������Դ·��
	virtual void SetObjectPath(const TArray<FSoftObjectPath> &InObjectPath) override final;

	//��ȡ��Դ·��
	virtual TArray<FSoftObjectPath> &GetObjectPath() override final;

	//��ȡ������
	virtual FStreamableManager *GetStreamableManager()override final;

private:
	TArray<FSoftObjectPath> SoftObjectPath; //��Դ·��
	FStreamableManager *StreamableManager;	//����
};
