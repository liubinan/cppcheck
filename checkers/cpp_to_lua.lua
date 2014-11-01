local f = assert(io.open('E:\\lbn\\cppcheck\\checkers\\tmp.c'))
local append_line
local if_pos
for line in f:lines() do
    --print(line)
	--print(string.match(line, '[%a_][%w_]*->[%a_][%w_]*%s*%('))
	-- trim tail space
	line = string.gsub(line, '%s+$', '')
	
    line = string.gsub(line, '(->)([%a_][%w_]*%s*%()', function (op, method)
		return ':' .. method
	end)
	
	line = string.gsub(line, '(->)([%a_][%w_]*)', function (op, abj)
		return '.' .. abj
	end)
	
	line = string.gsub(line, 'Token::', '')

	-- if with {
	line = string.gsub(line, '^(%s+if%s*%(.*%))%s*{$', function (ifstmt)
		return ifstmt .. ' then'
	end)
	line = string.gsub(line, '^(%s+else)%s+(if%s*%(.*%))%s*{$', function (elsestmt, ifstmt)
		return elsestmt .. ifstmt .. ' then'
	end)
	line = string.gsub(line, '^(%s+else)%s*{$', function (elsestmt)
		return elsestmt
	end)

	-- if with ;
	line = string.gsub(line, '^(%s+if%s*%(.*%))%s*(.*);$', function (ifstmt, blk)
		return ifstmt .. ' then ' .. blk .. '; end'
	end)

	
	-- if without {
	if string.match(line, '^(%s+if%s*%(.*%))$') or string.match(line, '^(%s+else)%s+(if%s*%(.*%))$') then
		line = string.gsub(line, '^(%s+if%s*%(.*%))$', function (ifstmt)
			return ifstmt .. ' then'
		end)
		line = string.gsub(line, '^(%s+else)%s+(if%s*%(.*%))$', function (elsestmt, ifstmt)
			return elsestmt .. ifstmt .. ' then'
		end)
		append_line = string.match(line, '^%s+') .. 'end'
		if_pos = 0
	end
	
	-- for with {
	line = string.gsub(line, '^(%s+for%s*%(.*%))%s*{$', function (forstmt)
		return forstmt .. ' do'
	end)
	line = string.gsub(line, '^(%s+else)%s+(for%s*%(.*%))%s*{$', function (elsestmt, forstmt)
		return elsestmt .. forstmt .. ' do'
	end)

	-- {
	line = string.gsub(line, '^(%s*){$', function (ss)
		return ''
	end)
	-- }
	line = string.gsub(line, '^(%s*)}$', function (ss)
		return ss .. 'end'
	end)
	
	-- //
	line = string.gsub(line, '//', '--')
	
	-- function
	line = string.gsub(line, '^%S+%s+(.*::)(%w+)%((.*)%)$', function (class, func, func_args)
		func_args = string.gsub(func_args, '[^,]+%s%*?([%a_][%w_]*)%s*,', function (farg)
			return farg .. ', '
		end)
		func_args = string.gsub(func_args, '[^,]+%s%*?([%a_][%w_]*)%s*$', function (farg)
			return farg
		end)
		return 'function ' .. func .. '(' .. func_args .. ')'
	end)

	-- ! || &&
	line = string.gsub(line, '!=', '~=')
	line = string.gsub(line, '!', 'not ')
	line = string.gsub(line, '%s*||%s*', ' or ')
	line = string.gsub(line, '%s*&&%s*', ' and ')
	
	line = string.gsub(line, 'Severity::', 'SeverityType.')
	
	if append_line and if_pos > 1 then
		if not string.match(line, 'else') then
			print(append_line)			
		end
		append_line = nil
		if_pos = nil
	end

	print(line)
	
	if_pos = if_pos and if_pos + 1
end
f:close()
