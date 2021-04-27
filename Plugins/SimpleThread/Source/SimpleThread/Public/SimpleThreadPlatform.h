#pragma once
#include "GenericPlatform/GenericPlatformProcess.h"

// #if PLATFORM_WINDOWS
// 	#include "Windows/WindowsPlatformProcess.h"
// #elif PLATFORM_LINUX
// 	#include "Linux/LinuxPlatformProcess.h"
// #elif PLATFORM_MAC
// 	#include "Mac/MacPlatformProcess.h"
// #elif PLATFORM_ANDROID
// 	#include "Mac/AndroidPlatformProcess.h"
// #endif


#if PLATFORM_WINDOWS
	#include "Windows/WindowsPlatformProcess.h"
#else

#if PLATFORM_LINUX
	#include "Linux/LinuxPlatformProcess.h"
#else

#if PLATFORM_MAC
	#include "Mac/MacPlatformProcess.h"
#else

#if PLATFORM_ANDROID
	#include "Mac/AndroidPlatformProcess.h"
#else
#endif
#endif
#endif
#endif 


