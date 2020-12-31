workspace "tui"
  location "makefiles"
  language "C++"
  architecture "x86_64"
  configurations { "debug", "release" }

  filter "configurations:debug"
  symbols "On"

  filter "configurations:release"
  optimize "on"

  filter {}
  buildoptions { "--std=c++2a"}



  objdir "build/obj/%{prj.name}/%{cfg.longname}"
  targetdir "build/bin/%{prj.name}/%{cfg.longname}"
  

project "vt-helper"
  kind "StaticLib"
  files "src/lib/vt/*"

project "lib"
  kind "StaticLib"
  files "src/lib/*"


function includeLib()
  includedirs "src/lib"
  filter "kind:not StaticLib"
  links "vt-helper"
  links "lib"
  links "fontconfig"
end

project "app"
  kind "ConsoleApp"
  files "src/app/*"
  includeLib()
