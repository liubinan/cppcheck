do
    function get_next_token(first, cur)
        if not cur then
            return first
        else
            return cur:next()
        end
    end
    
    function iter_tokens(start_tok, end_tok)
        if not end_tok then
            return get_next_token, start_tok, nil
        else
            local the_end_tok = end_tok
            return function(first, cur)
                local ret
                if not cur then
                    ret = first
                else
                    ret = cur:next()
                end
                if not IsSameToken(ret, the_end_tok) then
                    return ret
                end
            end, start_tok
        end
    end
end
