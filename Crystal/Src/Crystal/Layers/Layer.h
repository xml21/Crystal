#pragma once

#include "Crystal/Core/Core.h"
#include "Crystal/Events/Event.h"

/* Abstract base class for all layers that are going to be used in client */

namespace Crystal
{
	class CRYSTAL_API Layer
	{
	public:
		Layer();
		virtual ~Layer() = default;

		virtual void OnAttach() {};
		virtual void OnDetach() {};
		virtual void OnUpdate(float ) {};
		virtual void OnImGuiRender() {};
		virtual void OnEvent(Event& event) {}
	};
}

