/*
* Cppcheck - A tool for static C/C++ code analysis
* Copyright (C) 2007-2014 Daniel Marjamäki and Cppcheck team.
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "tokenize.h"
#include "luaplugin.h"
#include "testsuite.h"
#include "filelister.h"
#include <sstream>
#include <string>

#include "lua/fflua.h"
using namespace ff;

extern std::ostringstream errout;

void defTestFixture(lua_State* L);
void defTestLuaPlugin(lua_State* L);

class TestLuaPlugin : public TestFixture {
public:
    TestLuaPlugin(const std::string& test_name, const std::string& checker_name, const std::string& lua_file) 
		: TestFixture(test_name), checker_name(checker_name), lua_file(lua_file) {}

private:
	std::string lua_file;
	std::string checker_name;
public:
	std::string get_errout() {
		return errout.str();
	}
	void clean_errout() {
		errout.str("");
	}

	Check* get_checker_by_name(const std::string& check_name) {
		for (Check* check : Check::instances())
		{
			if (check->name() == check_name)
			{
				return check;
			}
		}
		return nullptr;
	}

public:
    void check(
        const char* code,
        const char *filename = NULL) {
        // Clear the error buffer..
        errout.str("");

        Settings settings;
        settings.addEnabled("all");

        // Tokenize..
        Tokenizer tokenizer(&settings, this);
        std::istringstream istr(code);
        tokenizer.tokenize(istr, filename && filename[0] ? filename : "test.cpp");

        //std::cout << code << std::endl;
        //tokenizer.dump(std::cout);

        // Check..
		Check* check = get_checker_by_name(checker_name);
		if (check)
		{
			check->runChecks(&tokenizer, &settings, this);
			tokenizer.simplifyTokenList2();
			check->runSimplifiedChecks(&tokenizer, &settings, this);
		}
    }

    void run() {
		fflua_t fflua;
		std::string exe_dir = Path::getPathFromFilename(Path::getModuleFileName());
		std::string lua_plugin_dir = exe_dir + "/checkers";
		fflua.add_package_path(lua_plugin_dir);

		LuaPlugin::regLuaClasses(fflua);
		fflua.reg(defTestFixture);
		fflua.reg(defTestLuaPlugin);
		
		fflua.set_global_variable("test_fixture", this);

		/*
		fflua.run_string("check = function(code, filename)"
						"    filename = filename or '';"
						"    test_fixture:check(code, filename);"
						"end"
						);
		fflua.run_string("ASSERT_EQUALS = function(expected, actual)"
						"    test_fixture:assertEquals(debug.getinfo(2).source, debug.getinfo(2).currentline, tostring(expected), tostring(actual), '');"
						"end");
						*/
		fflua.load_file(lua_file);

		if (fflua.is_table_exists("test_case"))
		{
			fflua.run_string("for test_name, check_func in pairs(test_case) do "
							"    test_fixture:prepareTest(test_name);"
							"    check_func();"
							"end");
		}
    }
};

void defTestFixture(lua_State* L)
{
	fflua_register_t<TestFixture, int()> test_fixtrue_reg(L, "TestFixture");
	test_fixtrue_reg.def(&TestFixture::classname, "classname");
	test_fixtrue_reg.def(&TestFixture::testToRun, "testToRun");
	test_fixtrue_reg.def(&TestFixture::gcc_style_errors, "gcc_style_errors");
	test_fixtrue_reg.def(&TestFixture::quiet_tests, "quiet_tests");
	test_fixtrue_reg.def(&TestFixture::currentTest, "currentTest");
	test_fixtrue_reg.def(&TestFixture::prepareTest, "prepareTest");
	test_fixtrue_reg.def(&TestFixture::assert_, "assert_");
	// not_imp test_fixtrue_reg.def(&TestFixture::todoAssert, "todoAssert");
	typedef void (TestFixture::*assertEqualsFunc_t)(const char *filename, unsigned int linenr, const std::string &expected, const std::string &actual, const std::string &msg) const;
	test_fixtrue_reg.def((assertEqualsFunc_t)&TestFixture::assertEquals, "assertEquals");
	test_fixtrue_reg.def(&TestFixture::assertEqualsDouble, "assertEqualsDouble");
	typedef void (TestFixture::*todoAssertEqualsFunc_t)(const char *filename, unsigned int linenr, const std::string &wanted,
		const std::string &current, const std::string &actual) const;
	test_fixtrue_reg.def((todoAssertEqualsFunc_t)&TestFixture::todoAssertEquals, "todoAssertEquals");
	test_fixtrue_reg.def(&TestFixture::assertThrowFail, "assertThrowFail");
	test_fixtrue_reg.def(&TestFixture::complainMissingLib, "complainMissingLib");
	//test_fixtrue_reg.def(&TestFixture::processOptions, "processOptions");
}

void defTestLuaPlugin(lua_State* L)
{
	fflua_register_t<TestLuaPlugin, int()> test_plugin_reg(L, "TestLuaPlugin", "TestFixture");
	test_plugin_reg.def(&TestLuaPlugin::get_errout, "get_errout");
	test_plugin_reg.def(&TestLuaPlugin::clean_errout, "clean_errout");
	test_plugin_reg.def(&TestLuaPlugin::get_checker_by_name, "get_checker_by_name");
	test_plugin_reg.def(&TestLuaPlugin::check, "check");
}

namespace {
	struct init_test_lua_plugin_t {
		init_test_lua_plugin_t() {
			std::string exe_dir = Path::getPathFromFilename(Path::getModuleFileName());
			std::string lua_plugin_dir = exe_dir + "/checkers";
			std::map<std::string, std::size_t> files;
			std::set<std::string> lua_extra;
			lua_extra.insert(".lua");

			FileLister::recursiveAddFiles(files, lua_plugin_dir, lua_extra);

			for (auto f : files)
			{
				fflua_t fflua;
				fflua.add_package_path(lua_plugin_dir);

				std::string lua_file = f.first;
				if (Path::getFilenameExtensionInLowerCase(lua_file) != ".lua") {
					continue;
				}
				fflua.load_file(lua_file);

				if (fflua.is_function_exists("checkName") 
					&& fflua.is_function_exists("testName")
					&& fflua.is_table_exists("test_case"))
				{
					std::string check_name = fflua.call<string>("checkName");
					std::cout << "checkName: " << check_name << std::endl;

					std::string test_name = fflua.call<string>("testName");
					std::cout << "testName: " << test_name << std::endl;
					
					new TestLuaPlugin(test_name, check_name, lua_file);
				}
			}

		}
	}init_test_lua_plugin;

}
