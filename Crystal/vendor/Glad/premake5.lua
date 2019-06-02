project "Glad"
    kind "StaticLib"
    language "C"
	staticruntime "On"
    
    targetdir ("Bin/" .. outputdir .. "/%{prj.name}")
    objdir ("Int/" .. outputdir .. "/%{prj.name}")

    files
    {
        "include/glad/glad.h",
        "include/KHR/khrplatform.h",
        "src/glad.c"
    }

	includedirs
	{
		"include"
	}
    
    filter "system:windows"
        systemversion "latest"
		
	filter "configurations:Debug"
		defines "CL_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "CL_RELEASE"
		runtime "Release"
		optimize "on"