require 'check_utils'
require 'test_utils'

do
    function myName()
        return "CheckSysTableUpdateTuple"
    end
    
    function findFuncPattern(start)
        return findmatch(start, "systable_update_tuple ( %any%", nil, 0);
    end
    
    function sameArgumentError(tok)
        literal_str = tok and tok:str() or "null"
        _checkPlugin:reportError(tok, SeverityType.error,
            "sameArgumentError", 
            "systable_update_tuple's formal parameter 'oldtup' and 'newtup' has same actual parameter: " .. literal_str .. " . see bug 12826 for more info \n" ..
            "systable_update_tuple's formal parameter 'oldtup' and 'newtup' has same actual parameter: " .. literal_str .. " . see bug 12826 for more info",
            false);
    end
    
    function runSimplifiedChecks()
        local tok = findFuncPattern(_tokenizer:tokens());
        local endTok = tok and (tok:next():link());

        while (tok and endTok) do
            local first_arg = tok:next():next();
            if first_arg then
                local second_arg = first_arg:nextArgument()
                if second_arg then
                    local third_arg = second_arg:nextArgument()
                    if third_arg then
                        local fourth_arg = third_arg:nextArgument()
                        if fourth_arg then
                            if second_arg:str() == third_arg:str() then
                                -- todo: check all arg tokens
                                sameArgumentError(second_arg)
                            end
                        end
                    end
                end
            end

            tok = findFuncPattern(endTok:next());
            endTok = tok and (tok:next():link());
        end
    end
    
    function testName()
        return "TestCheckSysTableUpdateTuple"
    end
    
    function checkName()
        return "CheckSysTableUpdateTuple"
    end
        
    test_case = {
        ["test1"] = function1() 
            check(
                [[
                int foo() {\n
                   (void)systable_update_tuple(idxsubrel, idxsubphtTupleOld, idxsubphtTupleNew, xid, TRUE, TRUE);\n
                }\n]]
                );
            ASSERT_EQUALS("", test_fixture:get_errout());

            check(
                [[
                int foo() {\n
                   (void)systable_update_tuple(idxsubrel, idxsubphtTuple, idxsubphtTuple, xid, TRUE, TRUE);\n
                }\n]]
                );
            ASSERT_EQUALS("[test.cpp:2]: (error) systable_update_tuple's formal parameter 'oldtup' and 'newtup' has same actual parameter: idxsubphtTuple . see bug 12826 for more info \n", test_fixture:get_errout());
        end
    }
    
end

