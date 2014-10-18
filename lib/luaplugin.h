//---------------------------------------------------------------------------
#ifndef luapluginH
#define luapluginH
//---------------------------------------------------------------------------

#include "config.h"
#include "check.h"

/// @addtogroup Checks
/// @{

/**
* @brief lua checker plugin
*/

class CPPCHECKLIB LuaPlugin : public Check {
public:
    LuaPlugin() : Check(myName()) {
    }

    LuaPlugin(const Tokenizer *tokenizer, const Settings *settings, ErrorLogger *errorLogger)
        : Check(myName(), tokenizer, settings, errorLogger) {
    }

    virtual void runSimplifiedChecks(const Tokenizer *tokenizer, const Settings *settings, ErrorLogger *errorLogger) {
        LuaPlugin check(tokenizer, settings, errorLogger);
        check.quoteElogStringArg();
    }

    void quoteElogStringArg();

public:
    void luaReportError(const Token *tok, const Severity::SeverityType severity, const char* id, const char* msg, bool inconclusive);

protected:
    static const Token* findElogPattern(const Token *start);

private:
    void quoteElogStringArgWith_SError(const Token *tok, const std::string& elogName);

    void getErrorMessages(ErrorLogger *errorLogger, const Settings *settings) const {
        LuaPlugin c(0, settings, errorLogger);
        c.quoteElogStringArgWith_SError(0, "LuaPlugin");
    }

    static std::string myName() {
        return "LuaPlugin";
    }

    std::string classInfo() const {
        return "Lua checker lugin.\n";
    }
};
/// @}
//---------------------------------------------------------------------------
#endif // luapluginH
