#if PLATFORM_WINDOWS
	#include <iostream>
	#include <thread>
	#define CPUThreadNumber std::thread::hardware_concurrency()
#else
	#define CPUThreadNumber 12
#endif


#define MUTEX_LOCL FScopeLock ScopeLock(&Mutex)


