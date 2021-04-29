#pragma  once
#include "Misc/ScopeLock.h"
#include "Abandonable/SimpleAbandonable.h"
#include "Async/TaskGraphInterfaces.h"

// #if PLATFORM_WINDOWS
// 	#include <iostream>
// 	#include <thread>
// 	#define CPUThreadNumber std::thread::hardware_concurrency()
// #elif PLATFORM_LINUX
// 	#define CPUThreadNumber 12
// #elif PLATFORM_MAC
// 	#define CPUThreadNumber 12
// #elif PLATFORM_ANDROID
// 	#define CPUThreadNumber 12
// #else
// 	#define CPUThreadNumber 12
// #endif

#if PLATFORM_WINDOWS
	#include <iostream>
	#include <thread>
	#define CPUThreadNumber std::thread::hardware_concurrency()
#else

#if PLATFORM_LINUX
	#define CPUThreadNumber 12
#else

#if PLATFORM_MAC
	#define CPUThreadNumber 12
#else

#if PLATFORM_ANDROID
	#define CPUThreadNumber 12
#else
	#define CPUThreadNumber 12
#endif
#endif
#endif
#endif

#define MUTEX_LOCL FScopeLock ScopeLock(&Mutex)


#define MUTEX_LOCL FScopeLock ScopeLock(&Mutex) 


//“Ï≤Ω
#define USE_UE_THREAD_POOL_ASYNCTASK(ThreadDelegate) \
(new FAutoDeleteAsyncTask<FSimpleAbandonable>(ThreadDelegate))->StartBackgroundTask()

#define ASYNCTASK_UOBJECT(Method,Object,...) \
USE_UE_THREAD_POOL_ASYNCTASK(FSimpleDelegate::CreateUObject(Object,Method,__VA_ARGS__))

#define ASYNCTASK_Raw(Object,Method,...) \
USE_UE_THREAD_POOL_ASYNCTASK(FSimpleDelegate::CreateRaw(Object,Method,__VA_ARGS__))

#define ASYNCTASK_SP(Object,Method,...) \
USE_UE_THREAD_POOL_ASYNCTASK(FSimpleDelegate::CreateSP(Object,Method,__VA_ARGS__))

#define ASYNCTASK_Lambda(Method,...) \
USE_UE_THREAD_POOL_ASYNCTASK(FSimpleDelegate::CreateLambda(Method,__VA_ARGS__))

#define ASYNCTASK_UFunction(Object,Method,...) \
USE_UE_THREAD_POOL_ASYNCTASK(FSimpleDelegate::CreateUFunction(Object,Method,__VA_ARGS__))

//Õ¨≤Ω
#define USE_UE_THREAD_POOL_SYNCTASK(ThreadDelegate) \
{FAsyncTask<FSimpleAbandonable> *SimpleAbandonable = new FAsyncTask<FSimpleAbandonable>(ThreadDelegate); \
SimpleAbandonable->StartBackgroundTask(); \
SimpleAbandonable->EnsureCompletion(); \
delete SimpleAbandonable; }

#define SYNCTASK_UOBJECT(Object,Method,...) \
USE_UE_THREAD_POOL_SYNCTASK(FSimpleDelegate::CreateUObject(Object,Method,__VA_ARGS__))

#define SYNCTASK_Raw(Object,Method,...) \
USE_UE_THREAD_POOL_SYNCTASK(FSimpleDelegate::CreateRaw(Object,Method,__VA_ARGS__))

#define SYNCTASK_SP(Object,Method,...) \
USE_UE_THREAD_POOL_SYNCTASK(FSimpleDelegate::CreateSP(Object,Method,__VA_ARGS__))

#define SYNCTASK_Lambda(Method,...) \
USE_UE_THREAD_POOL_SYNCTASK(FSimpleDelegate::CreateLambda(Method,__VA_ARGS__))

#define SYNCTASK_UFunction(Object,Method,...) \
USE_UE_THREAD_POOL_SYNCTASK(FSimpleDelegate::CreateUFunction(Object,Method,__VA_ARGS__))
