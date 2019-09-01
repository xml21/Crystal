#pragma once

/**
File containing all global includes for application classes
*/

#include "Crystal/Core/Application.h"

#include "Crystal/Layers/Layer.h"

#include "Crystal/Logging/Log.h"

#include "Crystal/Core/Core.h"

// --------- Input ------------
#include "Crystal/Input/Input.h"
#include "Crystal/Input/MouseButtonCodes.h"
#include "Crystal/Input/KeyCodes.h"
// ----------------------------

#include "Crystal/ImGui/ImGuiLayer.h"

// -------- Renderer ----------
#include "Crystal/Renderer/Renderer.h"
#include "Crystal/Renderer/RenderCommandQueue.h"
#include "Crystal/Renderer/Buffers/VertexBuffer.h"
#include "Crystal/Renderer/Buffers/IndexBuffer.h"
#include "Crystal/Renderer/Framebuffer.h"
#include "Crystal/Renderer/Shader.h"
#include "Crystal/Renderer/Mesh.h"
#include "Crystal/Renderer/Cameras/Camera.h"
#include "Crystal/Renderer/Textures/Texture.h"
#include "Crystal/Renderer/Textures/Texture2D.h"
#include "Crystal/Renderer/Textures/TextureCube.h"
#include "Crystal/Renderer/Cameras/OrthographicCamera.h"
// ----------------------------

#include "Crystal/Events/Event.h"

// ------- Entry Point --------
#include "Crystal/Core/EntryPoint.h"
// ----------------------------

// -------- ImGui -------------
#include "imgui/imgui.h"
// ----------------------------
