-- premake5.lua
workspace "YoutubeVulkan"
   architecture "x64"
   configurations { "Debug", "Release" }
   startproject "Editor"

   -- Workspace-wide build options for MSVC
   filter "system:windows"
      buildoptions { "/EHsc", "/Zc:preprocessor", "/Zc:__cplusplus" }

OutputDir = "%{cfg.system}-%{cfg.architecture}/%{cfg.buildcfg}"
projectDir = os.getcwd()

include "Editor/Build-Editor.lua"