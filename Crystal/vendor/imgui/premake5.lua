project "ImGui"
    kind "StaticLib"
    language "C++"
	cppdialect "C++17"
	staticruntime "on"
    
    targetdir ("Bin/" .. outputdir .. "/%{prj.name}")
    objdir ("Int/" .. outputdir .. "/%{prj.name}")

    files
    {
        "imconfig.h",
        "imgui.h",
        "imgui.cpp",
        "imgui_draw.cpp",
        "imgui_internal.h",
        "imgui_widgets.cpp",
        "imstb_rectpack.h",
        "imstb_textedit.h",
        "imstb_truetype.h",
        "imgui_demo.cpp"
    }
    
    filter "system:windows"
        systemversion "latest"

        
    filter { "system:windows", "configurations:Release" }
        buildoptions "/MT"
		
	filter "configurations:Debug"
		defines "CL_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "CL_RELEASE"
		runtime "Release"
		optimize "on"
