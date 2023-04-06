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
        debugdir ("bin/" .. outputdir .. "/%{prj.name}")
        defines "MZ_DEBUG"
        symbols "On"

    filter "configurations:Release"
        debugdir ("bin/" .. outputdir .. "/%{prj.name}")
        defines "MZ_RELEASE"
        optimize "On"

    filter "files:vendor/ImGui/**.cpp"
        flags {"NoPCH"}

    filter("files:**.hlsl")
        shaderobjectfileoutput("../bin/" .. outputdir .. "/%{prj.name}/Shaders/%{file.basename}.cso")

   filter("files:**_ps.hlsl")
      shadermodel("4.0")
      shadertype("Pixel")

      
   filter("files:**_vs.hlsl")
      shadermodel("4.0")
      shadertype("Vertex")

      
   filter("files:**_hs.hlsl")
      shadermodel("5.0")
      shadertype("Hull")

      
    filter("files:**_ds.hlsl")
      shadermodel("5.0")
      shadertype("Domain")
        