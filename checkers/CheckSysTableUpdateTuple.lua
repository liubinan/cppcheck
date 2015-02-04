require 'check_utils'
require 'test_utils'

do
    function myName()
        return "CheckSysTableUpdateTuple"
    end
    
    function findFuncPattern(start, endpos)
        return findmatch(start, "systable_update_tuple ( %any%", endpos, 0);
    end
    
    function sameArgumentError(tok)
        literal_str = tok and tok:str() or "null"
        _checkPlugin:reportError(tok, SeverityType.error,
            "sameArgumentError", 
            "systable_update_tuple's formal parameter 'oldtup' and 'newtup' has same actual parameter: " .. literal_str .. " . see bug 12826 for more info \n" ..
            "systable_update_tuple's formal parameter 'oldtup' and 'newtup' has same actual parameter: " .. literal_str .. " . see bug 12826 for more info",
            false);
    end
    
	function checkSysTableUpdateTuple(startPos, endPos)
		local tok = findFuncPattern(startPos, endPos);
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

            tok = findFuncPattern(endTok:next(), endPos);
            endTok = tok and (tok:next():link());
        end
    end
    
	function runSimplifiedChecks()
		local symbolDatabase = _tokenizer:getSymbolDatabase();
        if not symbolDatabase then
			return
		end
		local functions = symbolDatabase.functionScopes;
		if not functions then
			return
		end
		for i, scope in ipairs(functions) do
			if scope._function and scope._function.hasBody then -- We only look for functions with a body
				checkSysTableUpdateTuple(scope.classStart, scope.classEnd)
			end --while
		end --for
	end
	
    function testName()
        return "TestCheckSysTableUpdateTuple"
    end
    
    function checkName()
        return "CheckSysTableUpdateTuple"
    end
        
    test_case = {
        ["test1"] = function() 
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

