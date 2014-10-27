require "test_utils"

do
    function testName()
        return "TestAssert"
    end

    function checkName()
        return "Assert"
    end

    test_case = {
        safeFunctionCallInAssert = function()
            check(
               [[int a;
                 bool b = false;
                 int foo() {
                    if (b) { a = 1+2 };
                    return a;
                 }
                 assert(foo() == 3);
            ]]);
            ASSERT_EQUALS("", test_fixture:get_errout());

            check(
               [[int foo(int a) {
                     int b=a+1;
                     return b;
                 }
                 assert(foo(1) == 2);
            ]]);
            ASSERT_EQUALS("", test_fixture:get_errout());
        end;

        functionCallInAssert = function()
            check(
               [[int a;
                 int foo() {
                     a = 1+2;
                     return a;
                 }
                 assert(foo() == 3);
            ]]);
            ASSERT_EQUALS("[test.cpp:6]: (warning) Assert statement calls a function which may have desired side effects: 'foo'.\n", test_fixture:get_errout());

            --  Ticket #4937 "false positive: Assert calls a function which may have desired side effects"
            check([[struct SquarePack {
                      static bool isRank1Or8( Square sq ) {
                         sq &= 0x38;
                         return sq == 0 || sq == 0x38;
                       }
                   };
                   void foo() {
                      assert( !SquarePack::isRank1Or8(push2) );
                   }
                 ]]);
            ASSERT_EQUALS("", test_fixture:get_errout());

            check([[struct SquarePack {
                      static bool isRank1Or8( Square &sq ) {
                         sq &= 0x38;
                         return sq == 0 || sq == 0x38;
                       }
                   };
                   void foo() {
                      assert( !SquarePack::isRank1Or8(push2) );
                   }
                   ]]);
            ASSERT_EQUALS("[test.cpp:8]: (warning) Assert statement calls a function which may have desired side effects: 'isRank1Or8'.\n", test_fixture:get_errout());

            check([[struct SquarePack {
                      static bool isRank1Or8( Square *sq ) {
                         *sq &= 0x38;
                         return *sq == 0 || *sq == 0x38;
                       }
                   };
                   void foo() {
                      assert( !SquarePack::isRank1Or8(push2) );
                   }
                 ]]);
            ASSERT_EQUALS("[test.cpp:8]: (warning) Assert statement calls a function which may have desired side effects: 'isRank1Or8'.\n", test_fixture:get_errout());

            check([[struct SquarePack {
                      static bool isRank1Or8( Square *sq ) {
                         sq &= 0x38;
                         return sq == 0 || sq == 0x38;
                       }
                   };
                   void foo() {
                      assert( !SquarePack::isRank1Or8(push2) );
                   }
                   ]]);
            ASSERT_EQUALS("", test_fixture:get_errout());
        end;

        memberFunctionCallInAssert = function()
            check([[struct SquarePack {
                      void Foo();
                   };
                   void foo(SquarePack s) {
                      assert( s.Foo(); );
                   }
                   ]]);
            ASSERT_EQUALS("[test.cpp:5]: (warning) Assert statement calls a function which may have desired side effects: 'Foo'.\n", test_fixture:get_errout());

            check([[struct SquarePack {
                      void Foo() const;
                   };
                   void foo(SquarePack* s) {
                      assert( s->Foo(); );
                   }
                   ]]);
            ASSERT_EQUALS("", test_fixture:get_errout());

            check([[struct SquarePack {
                      static void Foo();
                   };
                   void foo(SquarePack* s) {
                      assert( s->Foo(); );
                   }
                   ]]);
            ASSERT_EQUALS("", test_fixture:get_errout());

            check([[struct SquarePack {
                   };
                   void foo(SquarePack* s) {
                      assert( s->Foo(); );
                   }
                   ]]);
            ASSERT_EQUALS("", test_fixture:get_errout());
        end;

        assignmentInAssert = function()
            check([[void f() {
                       int a; a = 0;
                       assert(a = 2);
                       return a;
                   }
                 ]]);
            ASSERT_EQUALS("[test.cpp:3]: (warning) Assert statement modifies 'a'.\n", test_fixture:get_errout());

            check([[void f(int a) {
                       assert(a == 2);
                       return a;
                   }
                 ]]);
            ASSERT_EQUALS("", test_fixture:get_errout());

            check([[void f(int a, int b) {
                       assert(a == 2 && b = 1);
                       return a;
                   }
                 ]]);
            ASSERT_EQUALS("[test.cpp:2]: (warning) Assert statement modifies 'b'.\n", test_fixture:get_errout());

            check([[void f() {
                       int a; a = 0;
                       assert(a += 2);
                       return a;
                   }
                 ]]);
            ASSERT_EQUALS("[test.cpp:3]: (warning) Assert statement modifies 'a'.\n", test_fixture:get_errout());

            check([[void f() {
                       int a; a = 0;
                       assert(a *= 2);
                       return a;
                   }
                 ]]);
            ASSERT_EQUALS("[test.cpp:3]: (warning) Assert statement modifies 'a'.\n", test_fixture:get_errout());

            check([[void f() {
                       int a; a = 0;
                       assert(a -= 2);
                       return a;
                   }
                 ]]);
            ASSERT_EQUALS("[test.cpp:3]: (warning) Assert statement modifies 'a'.\n", test_fixture:get_errout());

            check([[void f(int a) {
                       assert(a--);
                       return a;
                   }
                   ]]);
            ASSERT_EQUALS("[test.cpp:2]: (warning) Assert statement modifies 'a'.\n", test_fixture:get_errout());
        end;
    }
end