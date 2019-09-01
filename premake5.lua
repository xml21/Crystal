workspace "Crystal"
	architecture "x64"
	startproject "Sandbox"

	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- Include directories relative to root folder (solution directory)
IncludeDir = {}
IncludeDir["GLFW"] = "Crystal/Vendor/GLFW/include"
IncludeDir["Glad"] = "Crystal/Vendor/Glad/include"
IncludeDir["imgui"] = "Crystal/Vendor/imgui"
IncludeDir["glm"] = "Crystal/Vendor/glm"
IncludeDir["stb"] = "Crystal/Vendor/stb"


include "Crystal/Vendor/GLFW"
include "Crystal/Vendor/Glad"
include "Crystal/Vendor/imgui"

project "Crystal"
	location "Crystal"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir ("Bin/" .. outputdir .. "/%{prj.name}")
	objdir ("Int/" .. outputdir .. "/%{prj.name}")

	pchheader "Crpch.h"
	pchsource "Crpch.cpp"

	files
	{
		"%{prj.name}/Src/**.h",
		"%{prj.name}/Src/**.cpp",
		"%{prj.name}/Vendor/glm/glm/**.hpp",
		"%{prj.name}/Vendor/glm/glm/**.inl",
		"%{prj.name}/Vendor/stb/**.h",
		"%{prj.name}/Vendor/stb/**.cpp"
	}

	defines
	{
		"_CRT_SECURE_NO_WARNINGS"
	}

	includedirs
	{
		"%{prj.name}/Src",
		"%{prj.name}/Vendor",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.stb}",
        "%{prj.name}/Vendor/assimp/include"
	}

	links
	{
		"GLFW",
		"Glad",
		"ImGui",
		"opengl32.lib"
	}

	filter "system:windows"
		systemversion "latest"

		defines
		{
			"CL_PLATFORM_WINDOWS",
			"CL_BUILD_DLL",
			"GLFW_INCLUDE_NONE"
		}

	filter "configurations:Debug"
		defines "CL_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "CL_RELEASE"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		defines "CL_DIST"
		runtime "Release"
		optimize "on"

project "Sandbox"
	location "Sandbox"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir ("Bin/" .. outputdir .. "/%{prj.name}")
	objdir ("Int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"%{prj.name}/Src/**.h",
		"%{prj.name}/Src/**.cpp"
	}

	includedirs
	{
		"Crystal/Vendor/spdlog/include",
		"Crystal/Vendor",
		"Crystal/Src",
		"%{IncludeDir.glm}"
	}

	links
	{
		"Crystal",
		"Crystal/Vendor/assimp/win64/assimp.lib"
	}

	filter "system:windows"
		systemversion "latest"

		defines
		{
			"CL_PLATFORM_WINDOWS"
		}

	filter "configurations:Debug"
		defines "CL_DEBUG"
		runtime "Debug"
		symbols "on"

		links
		{
			"Crystal/Vendor/assimp/bin/Debug/assimp-vc141-mtd.lib"
		}

	filter "configurations:Release"
		defines "CL_RELEASE"
		runtime "Release"
		optimize "on"

		links
		{
			"Crystal/Vendor/assimp/bin/Debug/assimp-vc141-mtd.lib"
		}

	filter "configurations:Dist"
		defines "CL_DIST"
		runtime "Release"
		optimize "on"

		links
		{
			"Crystal/Vendor/assimp/bin/Debug/assimp-vc141-mtd.lib"
		}