#pragma  once
#include "Misc/ScopeLock.h" //没报错就没添加


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


