#pragma once

#include "Crystal/Layers/Layer.h"

#include "Crystal/Events/MouseEvent.h"
#include "Crystal/Events/KeyEvent.h"
#include "Crystal/Events/ApplicationEvent.h"

namespace Crystal {

	class CRYSTAL_API ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();
		~ImGuiLayer() = default;

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnImGuiRender() override;

		/* Start layer-specific operations, like for ex. rendering */
		void Begin();

		/* End layer-specific operations, like for ex. rendering */
		void End();
	};

}