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
    cppdialect "C++20"
    targetdir "bin/%{cfg.buildcfg}"

    files { "src/vendor/glad/src/glad.c", "src/vendor/imgui/*" , "src/**.hpp", "src/**.h", "src/**.cpp" }

    includedirs { "src/**", "Dependencies/glfw/include", "src/vendor/**" }
    
    libdirs { "src/vendor/glfw/lib/", "src/vendor/lua/lib/" }
    links {"glfw3", "lua"}

    filter "system:windows"                             
        links { "opengl32"}

    filter "system:linux"
        links { "dl", "GL" }

    filter "configurations:Debug"
        defines { "DEBUG" }
        symbols "On"

    filter "configurations:Release"
        defines { "NDEBUG" }
        optimize "On"