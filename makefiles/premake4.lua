
function getPlatDir(plat)
    if os.get() == "windows" then
        if plat == "x32" then
            return "win32"
        else
            return"win64"
        end
    elseif os.get() == "linux" then
        if plat == "x32" then
            return "linux32"
        else
            return "linux64"
        end
    elseif os.get() == "solaris" then
        return "solaris"
    end
    return "unkown"
end

function setTargetObjDir(outDir)
	for _, cfg in ipairs(configurations()) do
		for _, plat in ipairs(platforms()) do
			targetPath = outDir .. "/" .. string.lower(cfg) .. "/" .. getPlatDir(plat)
			
			configuration {cfg, plat}
				targetdir(targetPath)
		end
	end
end

function setLibDir(libDir)
    if (libDir == "" or libDir == nil) then
        libDir = "."
    end
    for _, plat in ipairs(platforms()) do
        configuration {plat}
            libdirs {
                libDir .. "/" .. getPlatDir(plat)
            }
    end
end

solution "cppcheck"
    -- 解决方案全局设置
    configurations {"Debug", "Release"}
    platforms {"x32", "x64"}
    location (os.get() .. "/" .. iif(_ACTION, _ACTION, "unkown"))
    flags {"StaticRuntime"}
    
    configuration "debug"
		defines { "DEBUG" }
		flags { "Symbols" }

	configuration "release"
		defines { "NDEBUG" }
		flags { "Optimize" }
    
    configuration "vs*"
        defines {"WIN32", "_CRT_SECURE_NO_WARNINGS"}
        buildoptions {"-wd4996"}
 
	project "lua"
        kind "StaticLib"
        language "C"
        files {
            "../lua/*.h", 
			"../lua/*.c"
        }
        includedirs {
            "../lua",
        }
        targetdir("bin")
		
		configuration {"gmake or codeblocks or codelite"}
			links {"dl"}
		
		configuration "vs*"
			defines {
				"LUA_WIN",
			}

    -- 设置cli工程
    project "cli"
		targetname "cppcheck"
        kind "ConsoleApp"
        language "C++"
        files { 
			"../cli/**.h", 
			"../cli/*.cpp",
			"../lib/*.h", 
			"../lib/*.cpp",
			"../externals/tinyxml/*.h",
			"../externals/tinyxml/*.cpp",
			"../externals/fflua/**.h",

		}
		excludes {"../lib/lua_ti*"}
        includedirs { 
			"../cli",
			"../lib",
			"../lua",
			"../externals/tinyxml",
			"../externals/fflua",
		}
        links {"lua"}
		targetdir("bin")

		configuration "vs*"
			defines {
				"LUA_WIN",
			}

		configuration {"gmake or codeblocks or codelite"}
			buildoptions {"-std=gnu++0x", "-include ../../../lib/cxx11emu.h"}
			links {"dl"}
		
		configuration {"windows"}
			links {"shlwapi"}

		configuration "windows"
		    postbuildcommands { 
				[[cmd /c "echo D | xcopy /y ..\\..\\..\\cfg ..\\..\\bin\\cfg"]],
				[[cmd /c "echo D | xcopy /y ..\\..\\..\\checkers ..\\..\\bin\\checkers"]],
				[[cmd /c "echo D | xcopy /y ..\\..\\..\\rules ..\\..\\bin\\rules"]],
			}
			
		configuration "not windows"
		    postbuildcommands { 
				[[cp -rf ../../../cfg ../../bin/]],
				[[cp -rf ../../../checkers ../../bin/]],
				[[cp -rf ../../../checkers ../../bin/]],
			}


	-- 测试工程
	project "testrunner"
        kind "ConsoleApp"
        language "C++"
        files { 
			"../test/*.h", 
			"../test/*.cpp",
			"../cli/*.h",
			"../cli/*.cpp",
			"../externals/fflua/**.h",
			"../lib/*.h", 
			"../lib/*.cpp",
			"../externals/tinyxml/*.h",
			"../externals/tinyxml/*.cpp",
			"../externals/fflua/**.h",

		}
		excludes {"../lib/lua_ti*"}
		excludes {"../cli/main.cpp"}
        includedirs { 
			"../lib",
			"../cli",
			"../externals/tinyxml",
			"../lua",
			"../externals/fflua",
		}
        links {"lua"}
        targetdir("bin")
		
		configuration "vs*"
			defines {
				"LUA_WIN",
			}

		configuration {"gmake or codeblocks or codelite"}
			buildoptions {"-std=gnu++0x", "-include ../../../lib/cxx11emu.h"}
			links {"dl"}
			
		configuration {"windows"}
			links {"shlwapi"}

		configuration "windows"
			postbuildcommands { 
				[[cmd /c "echo D | xcopy /y ..\\..\\..\\cfg ..\\..\\bin\\cfg"]],
				[[cmd /c "echo D | xcopy /y ..\\..\\..\\checkers ..\\..\\bin\\checkers"]],
				[[cmd /c "echo D | xcopy /y ..\\..\\..\\rules ..\\..\\bin\\rules"]],
			}
		 
		configuration "not windows"
		    postbuildcommands { 
				[[cp -rf ../../../cfg ../../bin/]],
				[[cp -rf ../../../checkers ../../bin/]],
				[[cp -rf ../../../checkers ../../bin/]],
			}

            