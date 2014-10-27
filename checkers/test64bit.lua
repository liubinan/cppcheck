require "test_utils"

do
    function testName()
        return "Test64BitPortability"
    end
    
    function checkName()
        return "64-bit portability"
    end
        
    test_case = {
        ["novardecl"] = function()
            -- if the variable declarations can't be seen then skip the warning
            check([[void foo()
                    {
                        a = p;
                    }]]);
            ASSERT_EQUALS("", test_fixture:get_errout());
        end
        ,

        ["functionpar"] = function()
            check([[int foo(int *p)
                  {
                      int a = p;
                      return a + 4;
                  }]]);
            ASSERT_EQUALS("[test.cpp:3]: (portability) Assigning a pointer to an integer is not portable.\n", test_fixture:get_errout());

            check([[int foo(int p[])
                    {
                       int a = p;
                       return a + 4;
                   }]]);
            ASSERT_EQUALS("[test.cpp:3]: (portability) Assigning a pointer to an integer is not portable.\n", test_fixture:get_errout());

            check([[int foo(int p[])
                   {
                       int *a = p;
                       return a;
                   }]]);
            ASSERT_EQUALS("[test.cpp:4]: (portability) Returning an address value in a function with integer return type is not portable.\n", test_fixture:get_errout());

            check([[void foo(int x)
                   {
                       int *p = x;
                       *p = 0;
                   }]]);
            ASSERT_EQUALS("[test.cpp:3]: (portability) Assigning an integer to a pointer is not portable.\n", test_fixture:get_errout());

            -- #4659
            check([[int f(const char *p) { 
                       return 6 + p[2] * 256;
                   }]]);
            ASSERT_EQUALS("", test_fixture:get_errout());
        end
        ,

        ["structmember"] = function()
            check([[struct Foo { int *p };
                   void f(struct Foo *foo) {
                       int i = foo->p;
                   }]]);
            TODO_ASSERT_EQUALS("[test.cpp:3]: (portability) Assigning a pointer to an integer is not portable.\n", "", test_fixture:get_errout());
        end
        ,

        ["ptrcompare"] = function()
            -- Ticket #2892
            check([[void foo(int *p) {
                       int a = (p != NULL);
                   }]]);
            ASSERT_EQUALS("", test_fixture:get_errout());
        end
        ,

        ["ptrarithmetic"] = function()
            -- #3073
            check([[void foo(int *p) {
                       int x = 10;
                       int *a = p + x;
                   }]]);
            ASSERT_EQUALS("", test_fixture:get_errout());

            check([[void foo(int *p) {
                       int x = 10;
                       int *a = x + p;
                   }]]);
            ASSERT_EQUALS("", test_fixture:get_errout());

            check([[void foo(int *p) {
                       int x = 10;
                       int *a = x * x;
                   }]]);
            TODO_ASSERT_EQUALS("error", "", test_fixture:get_errout());

            check([[void foo(int *start, int *end) {
                       int len;
                       int len = end + 10 - start;
                   }]]);
            ASSERT_EQUALS("", test_fixture:get_errout());
        end
        ,

        ["returnIssues"] = function()
            check([[void* foo(int i) {
                       return i;
                   }]]);
            ASSERT_EQUALS("[test.cpp:2]: (portability) Returning an integer in a function with pointer return type is not portable.\n", test_fixture:get_errout());

            check([[void* foo(int* i) {
                       return i;
                   }]]);
            ASSERT_EQUALS("", test_fixture:get_errout());

            check([[void* foo() {
                       return 0;
                   }]]);
            ASSERT_EQUALS("", test_fixture:get_errout());

            check([[int foo(int i) {
                       return i;
                   }]]);
            ASSERT_EQUALS("", test_fixture:get_errout());

            check([[int foo(char* c) {
                       return c;
                   }]]);
            ASSERT_EQUALS("[test.cpp:2]: (portability) Returning an address value in a function with integer return type is not portable.\n", test_fixture:get_errout());

            check([[int foo(char* c) {
                       return 1+c;
                   }]]);
            ASSERT_EQUALS("[test.cpp:2]: (portability) Returning an address value in a function with integer return type is not portable.\n", test_fixture:get_errout());

            check([[std::string foo(char* c) {
                       return c;
                   }]]);
            ASSERT_EQUALS("", test_fixture:get_errout());

            -- #4486
            check([[int foo(char *a, char *b) { 
                       return a + 1 - b;
                   }]]);
            ASSERT_EQUALS("", test_fixture:get_errout());

            -- 4642
            check([[struct s { 
                      int i;
                   };
                   int func(struct s *p) {
                    return 1 + p->i;
                   }]]);
            ASSERT_EQUALS("", test_fixture:get_errout());

            check([[static void __iomem *f(unsigned int port_no) {
                     void __iomem *mmio = hpriv->mmio;
                     return mmio + (port_no * 0x80);
                   }]]);
            ASSERT_EQUALS("", test_fixture:get_errout());
        end
        ,
    }
end