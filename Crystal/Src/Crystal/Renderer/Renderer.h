#pragma once

namespace Crystal
{
	enum class RendererAPI
	{
		NONE = 0,
		OpenGL = 1,
		DirectX = 2,
		Vulkan = 3
	};

	class Renderer
	{
	public:
		inline static RendererAPI GetAPI() { return sRendererAPI; };
		static void SetAPI(const RendererAPI NewAPI) { sRendererAPI = NewAPI; }

	private:
		static RendererAPI sRendererAPI;
	};
}
