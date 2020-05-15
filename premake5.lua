newaction {
	trigger = "assemble",
	description = "Pass this option to assemble all list files",
	execute = function()
		if _TARGET_OS == "windows" then
			os.execute("python autobuild -t lua")
		else
			os.execute("python3 autobuild -t lua")
		end
	end
 }

newaction {
   trigger     = "clean",
   description = "Clean build files",
   execute     = function ()
      os.rmdir("build")
   end
}

function find_llvm_link_dir()
	local file = io.popen("llvm-config --libdir")
	local output = file:read('*all')
	file:close()
	return output
end

function link_llvm()
	local file = io.popen("llvm-config --libnames")
	local output = file:read('*all')
	local libs = string.gsub(output, "%s", ";"):sub(1, -2)
	links
	{
		libs
	}
	file:close()
end

function link_llvm_unix()
	local file = io.popen("llvm-config --libnames")
	local output = file:read('*all')
	for lib in string.gmatch(output, "lib([^%s]+).a%s") do
		links
		{
			lib
		}
	end
	file:close()
end

function find_llvm_include_dir()
	local file = io.popen("llvm-config --includedir")
	local output = file:read('*all')
	file:close()
	return output
end

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
obj_dir = "build/obj/" .. outputdir .. "/%{prj.name}"
target_dir = "bin/" .. outputdir .. "/%{prj.name}"

-- Include directories relative to root folder (solution directory)
include_dir = {}
include_dir["glfw"] = "llvmes-gui/glfw/include"
include_dir["glad"] = "llvmes-gui/glad/include"
include_dir["imgui"] = "llvmes-gui/imgui"
include_dir["glm"] = "llvmes-gui/glm"
include_dir["spdlog"] = "llvmes-gui/spdlog/include"
include_dir["llvm"] = find_llvm_include_dir()

link_dir = {}
link_dir["llvm"] = find_llvm_link_dir()

workspace "LLVMES"
	architecture "x86_64"
	startproject "jit"
	location "build"

	configurations
	{
		"Debug",
		"Release"
	}
	
	flags
	{
		"MultiProcessorCompile"
	}

group "Dependencies"
	include "llvmes-gui/glfw"
	include "llvmes-gui/glad"
	include "llvmes-gui/imgui"

group ""

project "llvmes"
	location "build/%{prj.name}"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "off"

	targetdir (target_dir)
	objdir (obj_dir)

	files
	{
		"src/**.h",
		"src/**.cpp"
	}

	defines
	{
		"_CRT_SECURE_NO_WARNINGS"
	}

	includedirs
	{
		"src",
		"%{include_dir.llvm}"
	}

	filter "system:windows"
		systemversion "latest"

		defines
		{
		}

	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"

project "llvmes-gui"
	location "build/%{prj.name}"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "off"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir (obj_dir)

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	includedirs
	{
		"%{prj.name}/src",
		"%{include_dir.glm}",
		"%{include_dir.spdlog}",
		"%{include_dir.glfw}",
		"%{include_dir.imgui}",
		"%{include_dir.glad}"
	}

	filter "system:windows"
		systemversion "latest"
		
	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"

project "debugger"
	location "build/%{prj.name}"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "off"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir (obj_dir)

	libdirs { "%{link_dir.llvm}" }

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	links
	{
		"glfw",
		"glad",
		"imgui",
		"llvmes",
		"llvmes-gui"
	}

	includedirs
	{
		"src",
		"%{prj.name}",
		"llvmes-gui/src",
		"%{include_dir.glm}",
		"%{include_dir.spdlog}",
		"%{include_dir.glfw}",
		"%{include_dir.imgui}",
		"%{include_dir.glad}",
		"%{include_dir.llvm}"
	}

	postbuildcommands { "{COPY} %{wks.location}/../llvmes-gui/fonts/verdana.ttf %{wks.location}../" .. target_dir }
	filter "system:windows"
		systemversion "latest"
		links { link_llvm() }

	filter "system:macosx"
		links
		{
			"Cocoa.framework",
			"IOKit.framework",
			"Foundation.framework",
			"CoreServices.framework",
			"CoreVideo.framework",
			"OpenGL.framework"
		}

	filter "system:not windows"
		links { link_llvm_unix() }
		
	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"


function gen_test(name)
	project(name)
	location "build/test"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "off"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir (obj_dir)

	libdirs { "%{link_dir.llvm}" }

	files
	{
		"test/" .. name .. ".cpp"
	}

	links
	{
		"llvmes"
	}

	includedirs
	{
		"test",
		"src",
		"%{include_dir.llvm}",
	}
	
	filter "system:windows"
		systemversion "latest"
		links { link_llvm() }

	filter "system:not windows"
		links { link_llvm_unix(), "z", "curses" }
	
	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"
	
	filter "configurations:Release"
		runtime "Release"
		optimize "on"
end

group "Test"
	gen_test "interpreter"
	gen_test "jit"
	gen_test "memview"
group ""
