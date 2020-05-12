function find_llvm_link_dir()
	local file = io.popen("llvm-config --libdir")
	local output = file:read('*all')
	file:close()
	return output
end

function find_llvm_links()
	local file = io.popen("llvm-config --libnames")
	local output = file:read('*all')
	file:close()
	local res = string.gsub(output, "%s", ";")
	return res:sub(1, -2)
end

function find_llvm_include_dir()
	local file = io.popen("llvm-config --includedir")
	local output = file:read('*all')
	file:close()
	return output
end

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
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

link_libraries = {}
link_libraries["llvm"] = find_llvm_links()

workspace "LLVMES"
	architecture "x86_64"
	startproject "jit"

	newoption {
	   trigger     = "assemble",
	   description = "Pass this option to assemble all list files"
	}

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
	location "."
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "off"

	targetdir (target_dir)
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

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
	location "llvmes-gui"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "off"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

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
	location "debugger"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "off"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

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
		"llvmes-gui",
		"%{link_libraries.llvm}"
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

	postbuildcommands { "{COPY} %{wks.location}llvmes-gui/fonts/*.ttf %{wks.location}" .. target_dir }

	filter "system:windows"
		systemversion "latest"
		
	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"


function gen_test(name)
	project(name)
	location "test"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "off"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	libdirs { "%{link_dir.llvm}" }

	files
	{
		"test/" .. name .. ".cpp"
	}

	links
	{
		"%{link_libraries.llvm}",
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
