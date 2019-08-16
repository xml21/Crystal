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

		void PushLayer(std::shared_ptr<Layer> layer);
		void PushOverlay(std::shared_ptr<Layer> overlay);
		void PopLayer(std::shared_ptr<Layer> layer);
		void PopOverlay(std::shared_ptr<Layer> overlay);

		/* begin() & end() methods implementation allowing for range base interation over layers */
		std::vector<std::shared_ptr<Layer>>::iterator begin() { return mLayers.begin(); }
		std::vector<std::shared_ptr<Layer>>::iterator end() { return mLayers.end(); }

	private:
		std::vector<std::shared_ptr<Layer>> mLayers; // Member variable serving as a container for layers
		unsigned int mLayerInsertIndex = 0; // Position where layers end in stack and overlays start
	};
}

