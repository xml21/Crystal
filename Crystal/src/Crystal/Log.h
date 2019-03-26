#pragma once

#include "memory"

#include "Core.h"
#include "spdlog/spdlog.h"

/* Core log macros */
#define CL_CORE_LOG_CRITICAL(...)	::Crystal::Log::GetCoreLogger()->critical(__VA_ARGS__)
#define CL_CORE_LOG_ERROR(...)		::Crystal::Log::GetCoreLogger()->error(__VA_ARGS__)
#define CL_CORE_LOG_WARN(...)		::Crystal::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define CL_CORE_LOG_DEBUG(...)		::Crystal::Log::GetCoreLogger()->debug(__VA_ARGS__)
#define CL_CORE_LOG_INFO(...)		::Crystal::Log::GetCoreLogger()->info(__VA_ARGS__)
#define CL_CORE_LOG_TRACE(...)		::Crystal::Log::GetCoreLogger()->trace(__VA_ARGS__)

/* Client log macros */
#define CL_CLIENT_LOG_CRITICAL(...) ::Crystal::Log::GetClientLogger()->critical(__VA_ARGS__)
#define CL_CLIENT_LOG_ERROR(...)    ::Crystal::Log::GetClientLogger()->error(__VA_ARGS__)
#define CL_CLIENT_LOG_WARN(...)     ::Crystal::Log::GetClientLogger()->warn(__VA_ARGS__)
#define CL_CLIENT_LOG_DEBUG(...)    ::Crystal::Log::GetClientLogger()->debug(__VA_ARGS__)
#define CL_CLIENT_LOG_INFO(...)     ::Crystal::Log::GetClientLogger()->info(__VA_ARGS__)
#define CL_CLIENT_LOG_TRACE(...)    ::Crystal::Log::GetClientLogger()->trace(__VA_ARGS__)

namespace Crystal
{
	class CRYSTAL_API Log
	{
	public:
		static void Init();

		inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return mCoreLogger; }
		inline static std::shared_ptr<spdlog::logger>& GetClientLogger() { return mClientLogger; }

	private:
		static std::shared_ptr<spdlog::logger> mCoreLogger;
		static std::shared_ptr<spdlog::logger> mClientLogger;
	};
}

