require 'check_utils'

do
    function myName()
        return "CheckAssert"
    end
	
	function sideEffectInAssertError(tok,functionName)
        reportError(tok, SeverityType.warning,
                    "assertWithSideEffect", "Assert statement calls a function which may have desired side effects: '" .. functionName .. "'.\n" ..
                    "Non-pure function: '" .. functionName .. "' is called inside assert statement. " ..
                    "Assert statements are removed from release builds so the code inside " ..
                    "assert statement is not executed. If the code is needed also in release " ..
                    "builds, this is a bug.",
					false);
    end
    function assignmentInAssertError(tok,varname)
        reportError(tok, SeverityType.warning,
                    "assignmentInAssert", "Assert statement modifies '" .. varname .. "'.\n" ..
                    "Variable '" .. varname .. "' is modified insert assert statement. " ..
                    "Assert statements are removed from release builds so the code inside " ..
                    "assert statement is not executed. If the code is needed also in release " ..
                    "builds, this is a bug.",
					false);
    end
	
    -- checks if side effects happen on the variable prior to tmp
    function checkVariableAssignment(assignTok)
        v = assignTok:previous():variable();
        if (not v) then return; end
        -- assignment
        if (assignTok:isAssignmentOp() or assignTok:_type() == eIncDecOp) then
            if (v:isConst()) then return; end
            assignmentInAssertError(assignTok, v:name());
        end
        -- TODO: function calls on v
    end
    
    function hasReturnInSameScope(tok)
        -- TODO: even if a return is in the same scope, the assignment might not affect it.
        local cur_scope = tok:scope()
        if not cur_scope then
            return false
        end
        return findsimplematch(cur_scope.classStart, 'return', cur_scope.classEnd, 0)
    end

    function checkFunctionCall(tok)
        if (tok:_type() ~= TokenType.eFunction) then
            return
        end
        local f = tok:_function();
        if (f.nestedIn:isClassOrStruct() and not f.isStatic and not f.isConst) then
            sideEffectInAssertError(tok, f:name()); -- Non-const member function called
            return
        end
        local scope = f.functionScope;
        if (not scope) then
            return
        end
        for tok2 in iter_tokens(scope.classStart, scope.classEnd) do
            while true do
                if (tok2:_type() ~= TokenType.eAssignmentOp and tok2:type() ~= TokenType.eIncDecOp) then
                    break
                end
                local var = tok2:previous():variable();
                if (not var or var:isLocal() or (var:isArgument() and not var:isReference() and not var:isPointer())) then
                    break; -- See ticket #4937. Assigning function arguments not passed by reference is ok.
                end
                if (var:isArgument() and var:isPointer() and tok2:strAt(-2) ~= "*") then
                    break; -- Pointers need to be dereferenced, otherwise there is no error
                end
                
                -- why check return in same scope ?
                if (hasReturnInSameScope(tok2)) then
                    sideEffectInAssertError(tok, f:name());
                    return;
                end
            end
        end
    end
    
    function findAssertPattern(start)
        return findsimplematch(start, "assert|Assert|ASSERT ( ", nil, 0);
    end
    
    function assertWithSideEffects()
        if (not _settings:isEnabled("warning")) then
            return;
        end
        local tok = findAssertPattern(_tokenizer.tokens)
        local endTok = tok and (tok:next():link());
        while tok and endTok do
            for tmp in iter_tokens(tok:next(), endTok) do
                checkVariableAssignment(tmp);
                checkFunctionCall(tmp);
            end
            tok = findAssertPattern(endTok:next());
            endTok = tok and (tok:next():link());
        end
    end
    
	function runSimplifiedChecks()
        assertWithSideEffects()
    end
end
