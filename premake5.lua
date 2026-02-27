workspace "Paint"
    configurations { "Debug", "Release" }
    platforms { "x64" }  -- Specify x64 platform
    location "Build"  -- Generated files will go here

    -- Platform-specific settings
    filter "system:windows"
        architecture "x64"
        systemversion "latest"

    filter "system:linux"
        architecture "x64"
        toolset "gcc"

    filter {}

project "Paint"
    kind "ConsoleApp"
    language "C++"
    targetdir "bin/%{cfg.buildcfg}"

    files { "src/vendor/glad/src/glad.c", "src/**.hpp", "src/**.h", "src/**.cpp" }

    includedirs { "src/**", "src/vendor/**" }

    filter "system:windows"                             
        libdirs { "Dependencies/glfw/lib-vc2022", "Dependencies/glew/lib/Release/x64" }
        links { "glfw3", "opengl32" }

    filter "system:linux"
                links { "glfw", "dl",  "GL" }

    filter "configurations:Debug"
        defines { "DEBUG" }
        symbols "On"

    filter "configurations:Release"
        defines { "NDEBUG" }
        optimize "On"