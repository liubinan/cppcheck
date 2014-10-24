//---------------------------------------------------------------------------
#ifndef luapluginH
#define luapluginH
//---------------------------------------------------------------------------

#include "config.h"
#include "check.h"

#include "lua/fflua.h"

/// @addtogroup Checks
/// @{

/**
* @brief lua checker plugin
*/

class CPPCHECKLIB LuaPlugin : public Check {
public:
    LuaPlugin();

    LuaPlugin(const std::string& name, const std::string& luaFile, bool has_checks, bool has_simplified_checks)
        : Check(name), 
        luaFile_(luaFile),
        has_checks_(has_checks),
        has_simplified_checks_(has_simplified_checks){
    }

    LuaPlugin(const Tokenizer *tokenizer, const Settings *settings, ErrorLogger *errorLogger)
        : Check(myName(), tokenizer, settings, errorLogger) {
    }

    virtual void runSimplifiedChecks(const Tokenizer *tokenizer, const Settings *settings, ErrorLogger *errorLogger) {
        LuaPlugin check(tokenizer, settings, errorLogger);
        check.runSimplifiedChecks();
    }

    void runSimplifiedChecks();

public:
    void luaReportError(const Token *tok, const Severity::SeverityType severity, const char* id, const char* msg, bool inconclusive);
	void regLuaClasses(ff::fflua_t& fflua);
	
private:
    void getErrorMessages(ErrorLogger *errorLogger, const Settings *settings) const {
        LuaPlugin c(0, settings, errorLogger);
        //c.quoteElogStringArgWith_SError(0, "LuaPlugin");
    }

    static std::string myName() {
        return "LuaPlugin";
    }

    std::string classInfo() const {
        return "Lua checker lugin.\n";
    }

private:
    std::string luaFile_;
    bool has_checks_;
    bool has_simplified_checks_;
};
/// @}
//---------------------------------------------------------------------------
#endif // luapluginH
