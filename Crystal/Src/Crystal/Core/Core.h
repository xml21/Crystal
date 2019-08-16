#pragma once

/**
 * Preprocessor macro definitions
 */

#ifdef CL_PLATFORM_WINDOWS
#if CL_DYNAMIC_LINK
	#ifdef CL_BUILD_DLL
		#define CRYSTAL_API __declspec(dllexport)
	#else
		#define CRYSTAL_API __declspec(dllimport)
	#endif 
#else
	#define CRYSTAL_API
#endif
#else
	#error Crystal Engine currently supports Windows only!
#endif

#ifdef CL_ENABLE_ASSERTS
	#define CL_ASSERT(x, ...) { if(!x) { CL_ERROR("Assertion failed: {0}", __VA_ARGS__); __debugbreak(); } }
	#define CL_CORE_ASSERT(x, ...) { if(!x) { CL_CORE_ERROR("Assertion failed: {0}", __VA_ARGS__); __debugbreak(); } }
#else
	#define CL_ASSERT(x, ...)
	#define CL_CORE_ASSERT(x, ...)
#endif

#define BIT(x) (1 << x)

#define CL_BIND_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)