workspace "Crystal"
	architecture "x64"

	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

project "Crystal"
	location "Crystal"
	kind "SharedLib"
	language "C++"

	targetdir ("Bin/" .. outputdir .. "/%{prj.name}")
	objdir ("Int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"%{prj.name}/Src/**.h",
		"%{prj.name}/Src/**.cpp"
	}

	includedirs
	{
		"%{prj.name}/Vendor"
	}

	filter "system:windows"
		cppdialect "C++17"
		staticruntime "Off"
		systemversion "10.0.17763.0"

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
		symbols "On"

	filter "configurations:Release"
		defines "CL_RELEASE"
		optimize "On"

	filter "configurations:Dist"
		defines "CL_DIST"
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
		systemversion "10.0.17763.0"

		defines
		{
			"CL_PLATFORM_WINDOWS"
		}

	filter "configurations:Debug"
		defines "CL_DEBUG"
		symbols "On"

	filter "configurations:Release"
		defines "CL_RELEASE"
		optimize "On"

	filter "configurations:Dist"
		defines "CL_DIST"
		optimize "On"