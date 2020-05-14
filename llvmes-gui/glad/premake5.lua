-- delete a file if the clean action is running
if _ACTION == "clean" then
   os.rmdir("build")
end

project "glad"
    kind "StaticLib"
    language "C"
    staticruntime "on"
    objdir "build/obj"
    targetdir "build/bin"
    location "build"

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
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        runtime "Release"
        optimize "on"