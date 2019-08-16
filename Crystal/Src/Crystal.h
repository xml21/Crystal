#pragma once

/**
File containing all global includes for application classes
*/

#include "Crystal/Application.h"

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
#include "Crystal/Renderer/RenderCommand.h"
#include "Crystal/Renderer/Buffers/VertexBuffer.h"
#include "Crystal/Renderer/Buffers/IndexBuffer.h"
#include "Crystal/Renderer/Shader.h"
#include "Crystal/Renderer/VertexArray.h"
#include "Crystal/Renderer/Cameras/OrthographicCamera.h"
// ----------------------------

// ------- Entry Point --------
#include "Crystal/EntryPoint.h"
// ----------------------------