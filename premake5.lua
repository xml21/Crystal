workspace "Crystal"
	architecture "x64"

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

include "Crystal/Vendor/GLFW"

project "Crystal"
	location "Crystal"
	kind "SharedLib"
	language "C++"

	targetdir ("Bin/" .. outputdir .. "/%{prj.name}")
	objdir ("Int/" .. outputdir .. "/%{prj.name}")

	pchheader "Crpch.h"
	pchsource "Crpch.cpp"

	files
	{
		"%{prj.name}/Src/**.h",
		"%{prj.name}/Src/**.cpp"
	}

	includedirs
	{
		"%{prj.name}/Src",
		"%{prj.name}/Vendor",
		"%{IncludeDir.GLFW}"
	}

	links
	{
		"GLFW",
		"opengl32.lib"
	}

	filter "system:windows"
		cppdialect "C++17"
		staticruntime "Off"
		systemversion "latest"

		defines
		{
			"CL_PLATFORM_WINDOWS",
			"CL_BUILD_DLL"
		}

		postbuildcommands
		{
		 ("{COPY} %{cfg.buildtarget.relpath} \"../bin/" .. outputdir .. "/Sandbox/\"")
		}

	filter "configurations:Debug"
		defines "CL_DEBUG"
		buildoptions "/MDd"
		symbols "On"

	filter "configurations:Release"
		defines "CL_RELEASE"
		buildoptions "/MD"
		optimize "On"

	filter "configurations:Dist"
		defines "CL_DIST"
		buildoptions "/MD"
		optimize "On"

project "Sandbox"
	location "Sandbox"
	kind "ConsoleApp"
	language "C++"
	staticruntime "off"

	targetdir ("Bin/" .. outputdir .. "/%{prj.name}")
	objdir ("Int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"%{prj.name}/Src/**.h",
		"%{prj.name}/Src/**.cpp"
	}

	includedirs
	{
		"Crystal/Vendor",
		"Crystal/Src"
	}

	links
	{
		"Crystal"
	}

	filter "system:windows"
		cppdialect "C++17"
		staticruntime "On"
		systemversion "latest"

		defines
		{
			"CL_PLATFORM_WINDOWS"
		}

	filter "configurations:Debug"
		defines "CL_DEBUG"
		buildoptions "/MDd"
		symbols "On"

	filter "configurations:Release"
		defines "CL_RELEASE"
		buildoptions "/MD"
		optimize "On"

	filter "configurations:Dist"
		defines "CL_DIST"
		buildoptions "/MD"
		optimize "On"