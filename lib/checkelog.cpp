//---------------------------------------------------------------------------
// You should quote elog string param with _S() macro
//---------------------------------------------------------------------------

#include "checkelog.h"
#include "symboldatabase.h"

//---------------------------------------------------------------------------


// Register this check class (by creating a static instance of it)
namespace {
    CheckElog instance;
}

void CheckElog::assertWithSideEffects()
{
    //if (!_settings->isEnabled("warning"))
    //    return;

    const Token *tok = findElogPattern(_tokenizer->tokens());
    const Token *endTok = tok ? tok->next()->link() : nullptr;
 
    while (tok && endTok) {
        for (const Token* tmp = tok->next()->next(); tmp != endTok; tmp = tmp->next()) {
            if (tmp->str() == "(")
            {
                tmp = tmp->link();
                continue;
            }

            if (tmp->type() == Token::eString)
            {
                quoteElogStringArgWith_SError(tmp, tok->str());
            }
        }

        
        tok = findElogPattern(endTok->next());
        endTok = tok ? tok->next()->link() : nullptr;
    }
}
//---------------------------------------------------------------------------


void CheckElog::quoteElogStringArgWith_SError(const Token *tok, const std::string& elogName)
{
    reportError(tok, Severity::error,
        "quoteElogStringArgWith_S", 
        "You should quote literal string argument  " + tok->str() + "  of '" + elogName + "' with macro _S().\n"
        "You should quote literal string argument " + tok->str() + " with _S().");
}

const Token* CheckElog::findElogPattern(const Token* start)
{
    return Token::findmatch(start, "ELOGElog|elog_finish|ELOGSetLastError|ELOGSetLastError_ ( %any%");
}
