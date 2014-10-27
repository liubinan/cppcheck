do
    check = function(code, filename)
        filename = filename or '';
        test_fixture:check(code, filename);
    end
    
    ASSERT = function(cond)
        cond = cond and cond ~= 0 and true or false
        test_fixture:assert_(debug.getinfo(2).source, 
            debug.getinfo(2).currentline, 
            cond);
    end
    
    ASSERT_EQUALS = function(expected, actual)
        test_fixture:assertEquals(debug.getinfo(2).source, 
            debug.getinfo(2).currentline, 
            tostring(expected), 
            tostring(actual), 
            '');
    end
    
    ASSERT_EQUALS_MSG = function(expected, actual, msg)
        test_fixture:assertEquals(debug.getinfo(2).source, 
            debug.getinfo(2).currentline, 
            tostring(expected), 
            tostring(actual), 
            tostring(msg));
    end
    
    TODO_ASSERT_EQUALS = function(wanted, current, actual)
        test_fixture:todoAssertEquals(debug.getinfo(2).source, 
            debug.getinfo(2).currentline, 
            tostring(wanted), 
            tostring(current),
            tostring(actual));
    end
end