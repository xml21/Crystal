#pragma once

/**
File containing all global includes for application classes
*/

#include "Crystal/Application.h"
#include "Crystal/Layer.h"
#include "Crystal/Log.h"

// --------- Input ------------
#include "Crystal/Input.h"
#include "Crystal/MouseButtonCodes.h"
#include "Crystal/KeyCodes.h"
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