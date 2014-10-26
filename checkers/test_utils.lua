do
	check = function(code, filename)
		filename = filename or '';
		test_fixture:check(code, filename);
	end
	
	ASSERT_EQUALS = function(expected, actual)
		test_fixture:assertEquals(debug.getinfo(2).source, 
			debug.getinfo(2).currentline, 
			tostring(expected), 
			tostring(actual), 
			'');
	end
end