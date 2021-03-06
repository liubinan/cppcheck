require 'check_utils'
require 'test_utils'

do
    function myName()
        return "CheckElog"
    end
    
    function findElogPattern(start)
        return findmatch(start, "ELOGElog|elog_finish|ELOGSetLastError|ELOGSetLastError_ ( %any%", nil, 0);
    end
    
    function quoteElogStringArgWith_SError(tok, elogName)
        literal_str = tok and tok:str() or "null"
        _checkPlugin:reportError(tok, SeverityType.error,
            "quoteElogStringArgWith_S", 
            "You should quote literal string argument  " .. literal_str .. "  of '" .. elogName .. "' with macro _S().\n" ..
            "You should quote literal string argument  " .. literal_str .. "  of '" .. elogName .. "' with macro _S().",
            false);
    end
    
    function runSimplifiedChecks()
        local tok = findElogPattern(_tokenizer:tokens());
        local endTok = tok and (tok:next():link());

        while (tok and endTok) do
            local tmp = tok:next():next();
            while not IsSameToken(tmp, endTok) do
                if tmp:str() == "(" then
                    tmp = tmp:link();
                else
                    if tmp:type() == TokenType.eString then
                        quoteElogStringArgWith_SError(tmp, tok:str())
                    end
                    
                    tmp = tmp:next()
                end
            end
            
            tok = findElogPattern(endTok:next());
            endTok = tok and (tok:next():link());
        end
    end
    
    function testName()
        return "TestCheckElog"
    end
    
    function checkName()
        return "CheckElog"
    end
        
    test_case = {
        ["test1"] = function() 
            check(
                [[void elog_finish(int ecode, char* fmt, ...){}\n
                int foo() {\n
                   elog_finish(1,_S("%d"), 1);\n
                }\n]]
                );
            ASSERT_EQUALS("", test_fixture:get_errout());

            check(
                [[void elog_finish(int ecode, char* fmt, ...){}\n
                int foo() {\n
                   elog_finish(1,"%d", 1);\n
                }\n]]
                );
            ASSERT_EQUALS("[test.cpp:3]: (error) You should quote literal string argument  \"%d\"  of 'elog_finish' with macro _S().\n", test_fixture:get_errout());

            check(
                [[void elog_finish(int ecode, char* fmt, ...){}\n
                int foo() {\n
                   ELOGElog(1,"%d", 1);\n
                }\n]]
                );
            ASSERT_EQUALS("[test.cpp:3]: (error) You should quote literal string argument  \"%d\"  of 'ELOGElog' with macro _S().\n", test_fixture:get_errout());

        end
    }
    
end

