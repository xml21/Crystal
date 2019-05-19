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

	void Crystal::LayerStack::PushLayer(std::shared_ptr<Layer> layer)
	{
		mLayers.emplace(mLayers.begin() + mLayerInsertIndex, layer); //Insert at insert position (before overlays)
		mLayerInsertIndex++;
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
			mLayerInsertIndex--;
		}
	}

	void Crystal::LayerStack::PopOverlay(std::shared_ptr<Layer> layer)
	{
		auto it = std::find(mLayers.begin(), mLayers.end(), layer);

		if (it != mLayers.end())
			mLayers.erase(it);
	}

}
