workspace "Mizu"
    architecture "x64"

    configurations
    {
        "Debug",
        "Release"
    }

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

project "Mizu"
    location "Mizu"
    kind "WindowedApp"
    language "C++"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    pchheader "mzpch.h"
    pchsource "Mizu/src/mzpch.cpp"

    files
    {
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.cpp",
        "%{prj.name}/src/**.hlsl",
        "vendor/ImGui/**.cpp"
    }

    includedirs
    {
        "%{prj.name}/src",
        "$(SolutionDir)/vendor/spdlog/include",
        "$(SolutionDir)/vendor/ImGui"
    }

    links
    {
        "d3d11",
        "D3DCompiler",
        "gdiplus"
    }
    
    filter "system:windows"
        cppdialect "C++20"
        staticruntime "On"
        systemversion "latest"

        defines
        {
            "MZ_PLATFORM_WINDOWS"
        }

    filter "configurations:Debug"
        defines "MZ_DEBUG"
        symbols "On"

    filter "configurations:Release"
        defines "MZ_RELEASE"
        optimize "On"

    filter "files:vendor/ImGui/**.cpp"
        flags {"NoPCH"}

   filter("files:**_ps.hlsl")
      shadertype("Pixel")

   filter("files:**_vs.hlsl")
      shadertype("Vertex")