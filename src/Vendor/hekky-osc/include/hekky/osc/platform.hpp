#pragma once

#if _WIN32
	#ifdef _WIN64
		#define HEKKYOSC_WINDOWS
	#else
		#error "Hekky OSC does not support x86 (32-bit) Windows!"
	#endif
// Android is based on Linux, hence we have to check if the platform is Android before checking for Linux.
#elif defined(__android__)
	#define HEKKYOSC_ANDROID
	#error "Android is not supported!"
#elif defined(__linux__)
	#define HEKKYOSC_LINUX
	#error "Linux is not supported!"
#else
	// Sorry, we do not support whatever you're trying to use!
	#error "Unknown Platform!"
#endif

#if _DEBUG
	#define HEKKYOSC_DOASSERTS
#endif