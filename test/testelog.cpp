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
#include "checkelog.h"
#include "testsuite.h"
#include <sstream>

extern std::ostringstream errout;

class TestElog : public TestFixture {
public:
    TestElog() : TestFixture("TestElog") {}

private:
    void check(
        const char code[],
        const char *filename = NULL) {
        // Clear the error buffer..
        errout.str("");

        Settings settings;
        settings.addEnabled("warning");

        // Tokenize..
        Tokenizer tokenizer(&settings, this);
        std::istringstream istr(code);
        tokenizer.tokenize(istr, filename ? filename : "test.cpp");

        // Check..
        CheckElog CheckElog(&tokenizer, &settings, this);
        CheckElog.runSimplifiedChecks(&tokenizer, &settings, this);
    }

    void run() {
        TEST_CASE(elogQuoteStringArgWith_S);
    }


    void elogQuoteStringArgWith_S() {
        check(
            "void elog_finish(int ecode, char* fmt, ...){}\n"
            "int foo() {\n"
            "   elog_finish(1,_S(\"%d\"), 1);\n"
            "}\n"
        );
        ASSERT_EQUALS("", errout.str());

        check(
            "void elog_finish(int ecode, char* fmt, ...){}\n"
            "int foo() {\n"
            "   elog_finish(1,\"%d\", 1);\n"
            "}\n"
            );
        ASSERT_EQUALS("[test.cpp:3]: (error) You should quote literal string argument  \"%d\"  of 'elog_finish' with macro _S().\n", errout.str());
    }

};

REGISTER_TEST(TestElog)
