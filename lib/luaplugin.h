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

	/** @brief Run checks on the normal token list */
	void runChecks(const Tokenizer *tokenizer, const Settings *settings, ErrorLogger *errorLogger) {
		if (this->has_checks_)
		{
			LuaPlugin check(tokenizer, settings, errorLogger);
			check.luaFile_ = this->luaFile_;
			check.has_checks_ = this->has_checks_;
			check.has_simplified_checks_ = this->has_simplified_checks_;
			check.runChecks();
		}
	}

	/** @brief Run checks on the simplified token list */
    void runSimplifiedChecks(const Tokenizer *tokenizer, const Settings *settings, ErrorLogger *errorLogger) {
		if (this->has_simplified_checks_)
		{
			LuaPlugin check(tokenizer, settings, errorLogger);
			check.luaFile_ = this->luaFile_;
			check.has_checks_ = this->has_checks_;
			check.has_simplified_checks_ = this->has_simplified_checks_;
			check.runSimplifiedChecks();
		}
    }


public:
    void luaReportError(const Token *tok, const Severity::SeverityType severity, const char* id, const char* msg, bool inconclusive);
	static void regLuaClasses(ff::fflua_t& fflua);
	
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

	void runSimplifiedChecks();
	void runChecks();

private:
    std::string luaFile_;
    bool has_checks_;
    bool has_simplified_checks_;
};
/// @}
//---------------------------------------------------------------------------
#endif // luapluginH
