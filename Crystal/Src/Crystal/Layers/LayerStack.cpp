#include "Crpch.h"
#include "LayerStack.h"

namespace Crystal
{
	LayerStack::LayerStack()
	{
	}


	LayerStack::~LayerStack()
	{
	}

	void Crystal::LayerStack::PushLayer(Ref<Layer> layer)
	{
		mLayers.emplace(mLayers.begin() + mLayerInsertIndex, layer); //Insert at insert position (before overlays)
		mLayerInsertIndex++;
	}

	void Crystal::LayerStack::PushOverlay(Ref<Layer> overlay)
	{
		mLayers.emplace_back(overlay); //Insert at the end
	}

	void Crystal::LayerStack::PopLayer(Ref<Layer> layer)
	{
		auto it = std::find(mLayers.begin(), mLayers.end(), layer);
		if (it != mLayers.end())
		{
			layer->OnDetach();
			mLayers.erase(it);
			mLayerInsertIndex--;
		}
	}

	void Crystal::LayerStack::PopOverlay(Ref<Layer> overlay)
	{
		auto it = std::find(mLayers.begin(), mLayers.end(), overlay);

		if (it != mLayers.end())
		{
			overlay->OnDetach();
			mLayers.erase(it);
		}
	}

}
