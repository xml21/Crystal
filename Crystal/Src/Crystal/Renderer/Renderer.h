#pragma once

#include "RendererAPI.h"
#include "RenderCommandQueue.h"

#include "Cameras/OrthographicCamera.h"
#include "Shader.h"

class RenderCommand;
class VertexArray;

namespace Crystal
{
	struct SceneData
	{
		inline glm::mat4 GetViewProjectionMatrix() const { return ViewProjectionMatrix; }
		void SetViewProjectionMatrix(glm::mat4 val) { ViewProjectionMatrix = val; }

	private:
		glm::mat4 ViewProjectionMatrix;
	};

	class CRYSTAL_API Renderer
	{
	public:
		typedef void(*RenderCommandFn)(void*);

		static void Init();

		// Commands
		static void Clear();
		static void Clear(float r, float g, float b, float a = 1.0f);
		static void SetClearColor(float r, float g, float b, float a);

		static void DrawIndexed(unsigned int count, bool depthTest = true);

		static void* Submit(RenderCommandFn Fn, unsigned int Size)
		{
			return sInstance->mCommandQueue.Allocate(Fn, Size);
		}

		void WaitAndRender();

		inline static API GetAPI() { return RendererAPI::GetAPI(); };
		inline static Renderer& Get() { return *sInstance; }

		static Ref<SceneData> mSceneData;

	private:
		static Renderer* sInstance;

		RenderCommandQueue mCommandQueue;
	};

#define CL_RENDER_PASTE2(a, b) a ## b
#define CL_RENDER_PASTE(a, b) CL_RENDER_PASTE2(a, b)
#define CL_RENDER_UNIQUE(x) CL_RENDER_PASTE(x, __LINE__)

#define CL_RENDER(code) \
    struct CL_RENDER_UNIQUE(CLRenderCommand) \
    {\
        static void Execute(void*)\
        {\
            code\
        }\
    };\
	{\
		auto mem = ::Crystal::Renderer::Submit(CL_RENDER_UNIQUE(CLRenderCommand)::Execute, sizeof(CL_RENDER_UNIQUE(CLRenderCommand)));\
		new (mem) CL_RENDER_UNIQUE(CLRenderCommand)();\
	}\

#define CL_RENDER_1(arg0, code) \
	do {\
    struct CL_RENDER_UNIQUE(CLRenderCommand) \
    {\
		CL_RENDER_UNIQUE(CLRenderCommand)(typename ::std::remove_const<typename ::std::remove_reference<decltype(arg0)>::type>::type arg0) \
		: arg0(arg0) {}\
		\
        static void Execute(void* argBuffer)\
        {\
			auto& arg0 = ((CL_RENDER_UNIQUE(CLRenderCommand)*)argBuffer)->arg0;\
            code\
        }\
		\
		typename ::std::remove_const<typename ::std::remove_reference<decltype(arg0)>::type>::type arg0;\
    };\
	{\
		auto mem = ::Crystal::Renderer::Submit(CL_RENDER_UNIQUE(CLRenderCommand)::Execute, sizeof(CL_RENDER_UNIQUE(CLRenderCommand)));\
		new (mem) CL_RENDER_UNIQUE(CLRenderCommand)(arg0);\
	} } while(0)

#define CL_RENDER_2(arg0, arg1, code) \
    struct CL_RENDER_UNIQUE(CLRenderCommand) \
    {\
		CL_RENDER_UNIQUE(CLRenderCommand)(typename ::std::remove_const<typename ::std::remove_reference<decltype(arg0)>::type>::type arg0,\
											typename ::std::remove_const<typename ::std::remove_reference<decltype(arg1)>::type>::type arg1) \
		: arg0(arg0), arg1(arg1) {}\
		\
        static void Execute(void* argBuffer)\
        {\
			auto& arg0 = ((CL_RENDER_UNIQUE(CLRenderCommand)*)argBuffer)->arg0;\
			auto& arg1 = ((CL_RENDER_UNIQUE(CLRenderCommand)*)argBuffer)->arg1;\
            code\
        }\
		\
		typename ::std::remove_const<typename ::std::remove_reference<decltype(arg0)>::type>::type arg0;\
		typename ::std::remove_const<typename ::std::remove_reference<decltype(arg1)>::type>::type arg1;\
    };\
	{\
		auto mem = ::Crystal::Renderer::Submit(CL_RENDER_UNIQUE(CLRenderCommand)::Execute, sizeof(CL_RENDER_UNIQUE(CLRenderCommand)));\
		new (mem) CL_RENDER_UNIQUE(CLRenderCommand)(arg0, arg1);\
	}\

#define CL_RENDER_3(arg0, arg1, arg2, code) \
    struct CL_RENDER_UNIQUE(CLRenderCommand) \
    {\
		CL_RENDER_UNIQUE(CLRenderCommand)(typename ::std::remove_const<typename ::std::remove_reference<decltype(arg0)>::type>::type arg0,\
											typename ::std::remove_const<typename ::std::remove_reference<decltype(arg1)>::type>::type arg1,\
											typename ::std::remove_const<typename ::std::remove_reference<decltype(arg2)>::type>::type arg2) \
		: arg0(arg0), arg1(arg1), arg2(arg2) {}\
		\
        static void Execute(void* argBuffer)\
        {\
			auto& arg0 = ((CL_RENDER_UNIQUE(CLRenderCommand)*)argBuffer)->arg0;\
			auto& arg1 = ((CL_RENDER_UNIQUE(CLRenderCommand)*)argBuffer)->arg1;\
			auto& arg2 = ((CL_RENDER_UNIQUE(CLRenderCommand)*)argBuffer)->arg2;\
            code\
        }\
		\
		typename ::std::remove_const<typename ::std::remove_reference<decltype(arg0)>::type>::type arg0;\
		typename ::std::remove_const<typename ::std::remove_reference<decltype(arg1)>::type>::type arg1;\
		typename ::std::remove_const<typename ::std::remove_reference<decltype(arg2)>::type>::type arg2;\
    };\
	{\
		auto mem = ::Crystal::Renderer::Submit(CL_RENDER_UNIQUE(CLRenderCommand)::Execute, sizeof(CL_RENDER_UNIQUE(CLRenderCommand)));\
		new (mem) CL_RENDER_UNIQUE(CLRenderCommand)(arg0, arg1, arg2);\
	}\

#define CL_RENDER_4(arg0, arg1, arg2, arg3, code) \
    struct CL_RENDER_UNIQUE(CLRenderCommand) \
    {\
		CL_RENDER_UNIQUE(CLRenderCommand)(typename ::std::remove_const<typename ::std::remove_reference<decltype(arg0)>::type>::type arg0,\
											typename ::std::remove_const<typename ::std::remove_reference<decltype(arg1)>::type>::type arg1,\
											typename ::std::remove_const<typename ::std::remove_reference<decltype(arg2)>::type>::type arg2,\
											typename ::std::remove_const<typename ::std::remove_reference<decltype(arg3)>::type>::type arg3)\
		: arg0(arg0), arg1(arg1), arg2(arg2), arg3(arg3) {}\
		\
        static void Execute(void* argBuffer)\
        {\
			auto& arg0 = ((CL_RENDER_UNIQUE(CLRenderCommand)*)argBuffer)->arg0;\
			auto& arg1 = ((CL_RENDER_UNIQUE(CLRenderCommand)*)argBuffer)->arg1;\
			auto& arg2 = ((CL_RENDER_UNIQUE(CLRenderCommand)*)argBuffer)->arg2;\
			auto& arg3 = ((CL_RENDER_UNIQUE(CLRenderCommand)*)argBuffer)->arg3;\
            code\
        }\
		\
		typename ::std::remove_const<typename ::std::remove_reference<decltype(arg0)>::type>::type arg0;\
		typename ::std::remove_const<typename ::std::remove_reference<decltype(arg1)>::type>::type arg1;\
		typename ::std::remove_const<typename ::std::remove_reference<decltype(arg2)>::type>::type arg2;\
		typename ::std::remove_const<typename ::std::remove_reference<decltype(arg3)>::type>::type arg3;\
    };\
	{\
		auto mem = Renderer::Submit(CL_RENDER_UNIQUE(CLRenderCommand)::Execute, sizeof(CL_RENDER_UNIQUE(CLRenderCommand)));\
		new (mem) CL_RENDER_UNIQUE(CLRenderCommand)(arg0, arg1, arg2, arg3);\
	}

#define CL_RENDER_S(code) auto self = this;\
	CL_RENDER_1(self, code)

#define CL_RENDER_S1(arg0, code) auto self = this;\
	CL_RENDER_2(self, arg0, code)

#define CL_RENDER_S2(arg0, arg1, code) auto self = this;\
	CL_RENDER_3(self, arg0, arg1, code)

#define CL_RENDER_S3(arg0, arg1, arg2, code) auto self = this;\
	CL_RENDER_4(self, arg0, arg1, arg2, code)
}
