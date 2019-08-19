#pragma once

#include "Crystal/Core/Core.h"
#include "Layer.h"

#include <vector>

/*	Layer stack implementation.
*	This is generally a wrapper for std::vector.
*	Layer stack holds layers and overlays(they are always placed before layers).
	Example:
						 mLayerInsertPosition
								  v
*	Layer1 | Layer2 | Layer3 | Overlay1 | Overlay2 | Overlay3
 */

namespace Crystal
{
	class CRYSTAL_API LayerStack
	{
	public:
		LayerStack();
		~LayerStack();

		void PushLayer(Ref<Layer> layer);
		void PushOverlay(Ref<Layer> overlay);
		void PopLayer(Ref<Layer> layer);
		void PopOverlay(Ref<Layer> overlay);

		/* begin() & end() methods implementation allowing for range base interation over layers */
		std::vector<Ref<Layer>>::iterator begin() { return mLayers.begin(); }
		std::vector<Ref<Layer>>::iterator end() { return mLayers.end(); }

	private:
		std::vector<Ref<Layer>> mLayers; // Member variable serving as a container for layers
		unsigned int mLayerInsertIndex = 0; // Position where layers end in stack and overlays start
	};
}

