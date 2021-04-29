#include "CoreMinimal.h"
#include "Interface/ThreadManageInterface.h"


//Bind   ͬ���� �����������߳� �������󼤻������߳�
//Create �첽�� ֱ��������������ɺ��Զ�����
struct SIMPLETHREAD_API FThreadAbandonableManage :IAbandonableContainer
{

	//�첽��
	template<class UserClass, typename... VarTypes>
	void CtreateAsynchUObject(UserClass* TargetClass, typename TMemFunPtrType<false, UserClass, void(VarTypes...)>::Type InMethod, VarTypes... Vars)
	{
		return *this >> FSimpleDelegate::CreateUObject(TargetClass, InMethod, Vars...);
	}

	template<class UserClass, typename... VarTypes>
	void CreateAsynchRaw(UserClass* TargetClass, typename TMemFunPtrType<false, UserClass, void(VarTypes...)>::Type InMethod, VarTypes... Vars)
	{
		return *this >> FSimpleDelegate::CreateRaw(TargetClass, InMethod, Vars...);
	}

	template<class UserClass, typename... VarTypes>
	void CreateAsynchUFunction(UserClass* TargetClass, const FName& InMethodName, VarTypes... Vars)
	{
		return *this >> FSimpleDelegate::CreateUFunction(TargetClass, InMethodName, Vars...);
	}

	template<typename FuncType, typename... VarTypes>
	void CreateAsynchLambda(FuncType&& InMethod, VarTypes... Vars)
	{
		return *this >> FSimpleDelegate::CreateLambda(InMethod, Vars...);
	}

	template<class UserClass, typename... VarTypes>
	void CtreateAsynchSP(const TSharedRef<UserClass>& TargetClass, typename TMemFunPtrType<false, UserClass, void(VarTypes...)>::Type InMethod, VarTypes... Vars)
	{
		return *this >> FSimpleDelegate::CreateSP(TargetClass, InMethod, Vars...);
	}



	//ͬ����
	template<class UserClass, typename... VarTypes>
	void CtreateSynchUObject(UserClass* TargetClass, typename TMemFunPtrType<false, UserClass, void(VarTypes...)>::Type InMethod, VarTypes... Vars)
	{
		return *this << FSimpleDelegate::CreateUObject(TargetClass, InMethod, Vars...);
	}


	template<class UserClass, typename... VarTypes>
	void CreateSynchRaw(UserClass* TargetClass, typename TMemFunPtrType<false, UserClass, void(VarTypes...)>::Type InMethod, VarTypes... Vars)
	{
		return *this << FSimpleDelegate::CreateRaw(TargetClass, InMethod, Vars...);
	}

	template<class UserClass, typename... VarTypes>
	void CreateSynchUFunction(UserClass* TargetClass, const FName& InMethodName, VarTypes... Vars)
	{
		return *this << FSimpleDelegate::CreateUFunction(TargetClass, InMethodName, Vars...);
	}

	template<typename FuncType, typename... VarTypes>
	void CreateSynchLambda(FuncType&& InMethod, VarTypes... Vars)
	{
		return *this << FSimpleDelegate::CreateLambda(InMethod, Vars...);
	}

	template<class UserClass, typename... VarTypes>
	void CtreateSynchSP(const TSharedRef<UserClass>& TargetClass, typename TMemFunPtrType<false, UserClass, void(VarTypes...)>::Type InMethod, VarTypes... Vars)
	{
		return *this << FSimpleDelegate::CreateSP(TargetClass, InMethod, Vars...);
	}
};