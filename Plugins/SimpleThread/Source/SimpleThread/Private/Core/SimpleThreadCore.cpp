#include "Core/SimpleThreadType.h"
#include "Interface/ProxyInterface.h"
/* ����ΨһID*/
uint64 SpawnUniqueID() 
{
	return FMath::Abs(FMath::RandRange(100, 1245678) + FDateTime::Now().GetTicks());
}

FSimpleThreadHandle::FSimpleThreadHandle()
	:GUIDTypeA(SpawnUniqueID())
	,GUIDTypeB(SpawnUniqueID())
	,GUIDTypeC(SpawnUniqueID())
	,GUIDTypeD(SpawnUniqueID())
{
	
}

IThreadProxy::IThreadProxy()
	:SimpleThreadHandle(MakeShareable(new FSimpleThreadHandle))
{

}