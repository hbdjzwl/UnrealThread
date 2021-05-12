#pragma once
#include "CoreMinimal.h"
#include "Interface/ThreadManageInterface.h"
#include "Core/Manage/ManageBase/ThreadManageBase.h"

//Create 可以绑定异步结束后通知的事件
struct SIMPLETHREAD_API FResourceLoadingManage :FThreadTemplateBase<IStreamableContainer,TSharedPtr<struct FStreamableHandle>>
{
	typedef FThreadTemplateBase<
		IStreamableContainer,
		TSharedPtr<struct FStreamableHandle>> Super;

	FResourceLoadingManage();
	~FResourceLoadingManage();

protected:

	//设置资源路径
	virtual void SetObjectPath(const TArray<FSoftObjectPath> &InObjectPath) final;

	//获取资源路径
	virtual TArray<FSoftObjectPath> &GetObjectPath() final;

	//获取流管理
	virtual FStreamableManager *GetStreamableManager()final;

private:
	TArray<FSoftObjectPath> SoftObjectPath; //资源路径
	FStreamableManager *StreamableManager;	//管理
};
