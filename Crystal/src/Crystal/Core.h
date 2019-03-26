#pragma once

/**
 * Preprocessor macro definitions
 */

#ifdef CL_PLATFORM_WINDOWS
	#ifdef CL_BUILD_DLL
		#define CRYSTAL_API __declspec(dllexport)
	#else
		#define CRYSTAL_API __declspec(dllimport)
	#endif 
	#else
		#error Crystal Engine currently supports Windows only!
#endif