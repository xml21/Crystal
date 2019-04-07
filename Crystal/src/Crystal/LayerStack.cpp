#include "Crpch.h"
#include "LayerStack.h"

namespace Crystal
{
	LayerStack::LayerStack()
	{
		mLayerInsertPosition = mLayers.begin();
	}


	LayerStack::~LayerStack()
	{
	}

	void Crystal::LayerStack::PushLayer(std::shared_ptr<Layer> layer)
	{
		mLayerInsertPosition = mLayers.emplace(mLayerInsertPosition, layer); //Insert at insert position (before overlays)
	}

	void Crystal::LayerStack::PushOverlay(std::shared_ptr<Layer> overlay)
	{
		mLayers.emplace_back(overlay); //Insert at the end
	}

	void Crystal::LayerStack::PopLayer(std::shared_ptr<Layer> layer)
	{
		auto it = std::find(mLayers.begin(), mLayers.end(), layer);

		if (it != mLayers.end())
		{
			mLayers.erase(it);
			mLayerInsertPosition--;
		}
	}

	void Crystal::LayerStack::PopOverlay(std::shared_ptr<Layer> layer)
	{
		auto it = std::find(mLayers.begin(), mLayers.end(), layer);

		if (it != mLayers.end())
			mLayers.erase(it);
	}

}
