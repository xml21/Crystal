#pragma once

#include "glm/glm.hpp"

#include "Crystal/Renderer/RendererAPI.h"

namespace Crystal
{
	using RendererID = uint32_t;

	enum class API
	{
		NONE = 0, OpenGL = 1
	};

	struct APICapabilities
	{
		std::string Vendor;
		std::string Renderer;
		std::string Version;

		float MaxAnisotropy;
		int MaxSamples;
	};

	class RendererAPI
	{
	public:
		static void Init();
		static void Shutdown();

		static void SetClearColor(const glm::vec4& color);
		static void Clear(const glm::vec4& color);

		/* Draw call request supplied by vertex array */
		static void DrawIndexed(unsigned int Count, bool DepthTest = true);

		inline static APICapabilities& GetCapabilities()
		{
			static APICapabilities Capabilities;
			return Capabilities;
		}

		inline static API GetAPI() { return sAPI; }

	private:
		static API sAPI;
	};
}