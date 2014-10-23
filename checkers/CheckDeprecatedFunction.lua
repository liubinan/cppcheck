
do
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
	
	function myName()
		return "CheckDeprecatedFunction"
	end
	
	function runSimplifiedChecks()
		tok = findElogPattern(_tokenizer:tokens());
		endTok = tok and (tok:next():link());

		while (tok and endTok) do
			tmp = tok:next():next();
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
	
end

