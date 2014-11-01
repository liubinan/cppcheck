require 'check_utils'

do
    function myName()
        return "check64bit"
    end
    
    -- Is given variable a pointer or array? 
    local function isaddr(var)
        return (var and (var:isPointer() or var:isArray()));
    end

    -- Is given variable an integer variable
    local function isint(var)
        return (var and var:isIntegralType() and not var:isArrayOrPointer());
    end
    
    local function assignmentAddressToIntegerError(tok)
        _checkPlugin:reportError(tok, SeverityType.portability,
                    "AssignmentAddressToInteger",
                    "Assigning a pointer to an integer is not portable.\n" ..
                    "Assigning a pointer to an integer (int/long/etc) is not portable across different platforms and " ..
                    "compilers. For example in 32-bit Windows and linux they are same width, but in 64-bit Windows and linux " ..
                    "they are of different width. In worst case you end up assigning 64-bit address to 32-bit integer. The safe " ..
                    "way is to store addresses only in pointer types (or typedefs like uintptr_t).",
                    false);
    end

    local function assignmentIntegerToAddressError(tok)
        _checkPlugin:reportError(tok, SeverityType.portability,
                    "AssignmentIntegerToAddress",
                    "Assigning an integer to a pointer is not portable.\n" ..
                    "Assigning an integer (int/long/etc) to a pointer is not portable across different platforms and " ..
                    "compilers. For example in 32-bit Windows and linux they are same width, but in 64-bit Windows and linux " ..
                    "they are of different width. In worst case you end up assigning 64-bit integer to 32-bit pointer. The safe " ..
                    "way is to store addresses only in pointer types (or typedefs like uintptr_t).", 
                    false);
    end

    local function returnPointerError(tok)
        _checkPlugin:reportError(tok, SeverityType.portability,
                    "CastAddressToIntegerAtReturn",
                    "Returning an address value in a function with integer return type is not portable.\n" ..
                    "Returning an address value in a function with integer (int/long/etc) return type is not portable across " ..
                    "different platforms and compilers. For example in 32-bit Windows and Linux they are same width, but in " ..
                    "64-bit Windows and Linux they are of different width. In worst case you end up casting 64-bit address down " ..
                    "to 32-bit integer. The safe way is to always return an integer.",
                    false);
    end

    local function returnIntegerError(tok)
        _checkPlugin:reportError(tok, SeverityType.portability,
                    "CastIntegerToAddressAtReturn",
                    "Returning an integer in a function with pointer return type is not portable.\n" ..
                    "Returning an integer (int/long/etc) in a function with pointer return type is not portable across different " ..
                    "platforms and compilers. For example in 32-bit Windows and Linux they are same width, but in 64-bit Windows " ..
                    "and Linux they are of different width. In worst case you end up casting 64-bit integer down to 32-bit pointer. " ..
                    "The safe way is to always return a pointer.",
                    false);
    end

    local function pointerassignment()
        if not _settings:isEnabled("portability") then
            return
        end

        local symbolDatabase = _tokenizer:getSymbolDatabase();

        -- Check return values
        local functions = symbolDatabase.functionScopes;

        for i, scope in ipairs(functions) do
            while scope._function and scope._function.hasBody do -- We only look for functions with a body
                local retPointer = false;
                if scope._function.token:strAt(-1) == "*" then -- Function returns a pointer
                    retPointer = true;
                elseif (Match(scope._function.token:previous(), "int|long|DWORD", 0)) then -- Function returns an integer
                    retPointer = false;
                else
                    break;
                end

                for tok in iter_tokens(scope.classStart:next(), scope.classEnd) do
                    if Match(tok, "return %var%|%num% [;+]", 0) and not simpleMatch(tok, "return 0 ;") then
                        local ValType = { NO=1, INT=2, PTR=3, PTRDIFF=4 };
                        local val_type = ValType.NO;
        
                        for tok2 in iter_tokens(tok:next()) do
                            if ((val_type == ValType.NO or val_type == ValType.INT) and Match(tok2, "%var% [+;]", 0) and isaddr(tok2:variable())) then
                                val_type = ValType.PTR;
                            elseif (val_type == ValType.NO and (tok2:isNumber() or isint(tok2:variable()))) then
                                val_type = ValType.INT;
                            elseif (val_type == ValType.PTR and Match(tok2, "- %var%", 0) and isaddr(tok2:next():variable())) then
                                val_type = ValType.PTRDIFF;
                            elseif (tok2:str() == "(") then
                                -- TODO: handle parentheses
                                val_type = ValType.NO;
                                break;
                            elseif (val_type == ValType.PTR and simpleMatch(tok2, ".")) then
                                val_type = ValType.NO; -- Reset after pointer reference, see #4642
                            elseif (tok2:str() == ";") then
                                break;
                            end -- if
                        end -- for

                        if (retPointer and (val_type == ValType.INT or val_type == ValType.PTRDIFF)) then
                            returnIntegerError(tok);
                        elseif (not retPointer and val_type == ValType.PTR) then
                            returnPointerError(tok);
                        end --if
                    end -- if
                end -- while not IsSameToken(tok, scope.classEnd)
                break
            end -- while scope._function and scope._function.hasBody 
        end

        -- Check assignments
        for i, scope in ipairs(functions) do
            for tok in iter_tokens(scope.classStart, scope.classEnd) do
                if (Match(tok, "[;{}] %var% = %var% [;+]", 0)) then

                    local var1 = tok:next():variable();
                    local var2 = tok:tokAt(3):variable();

                    if (isaddr(var1) and isint(var2) and tok:strAt(4) ~= "+") then
                        assignmentIntegerToAddressError(tok:next());
                    elseif (isint(var1) and isaddr(var2)  and not tok:tokAt(3):isPointerCompare()) then
                        -- assigning address => warning
                        -- some trivial addition => warning
                        if (not Match(tok:tokAt(4), "+ %any% !!;", 0)) then
                            assignmentAddressToIntegerError(tok:next());
                        end
                    end
                end
            end
        end -- for
    end -- function
    
    function runChecks()
        pointerassignment()
    end

end
