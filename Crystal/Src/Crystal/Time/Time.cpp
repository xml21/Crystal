#include "Crpch.h"
#include "Time.h"

#include "Platform/Windows/WindowsTime.h"

namespace Crystal
{
	std::shared_ptr<Time> Time::Create()
	{
		#ifdef CL_PLATFORM_WINDOWS
			return std::make_shared<WindowsTime>();
		#endif

		CL_CORE_ASSERT(false, "Unknown Platform!");
			return nullptr;
	}

	Time::Time()
	{

	}
}
