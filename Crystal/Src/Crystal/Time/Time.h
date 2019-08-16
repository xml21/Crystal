#pragma once

#include "Crystal/Core/Core.h"

/* Abstract base class for all time related classes on all platforms that are going to be used in client */

namespace Crystal
{
	class CRYSTAL_API Time
	{
	public:
		Time();
		virtual ~Time() = default;

		virtual float GetSeconds() const = 0;
		virtual float GetMilliSeconds() const = 0;
		virtual void OnUpdate() = 0;

		static std::shared_ptr<Time> Create();
	};
}
