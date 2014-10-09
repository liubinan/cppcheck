//---------------------------------------------------------------------------
#ifndef checkelogH
#define checkelogH
//---------------------------------------------------------------------------

#include "config.h"
#include "check.h"

/// @addtogroup Checks
/// @{

/**
* @brief Checking for elog usage
*/

class CPPCHECKLIB CheckElog : public Check {
public:
    CheckElog() : Check(myName()) {
    }

    CheckElog(const Tokenizer *tokenizer, const Settings *settings, ErrorLogger *errorLogger)
        : Check(myName(), tokenizer, settings, errorLogger) {
    }

    virtual void runSimplifiedChecks(const Tokenizer *tokenizer, const Settings *settings, ErrorLogger *errorLogger) {
        CheckElog check(tokenizer, settings, errorLogger);
        check.quoteElogStringArg();
    }

    void quoteElogStringArg();

protected:
    static const Token* findElogPattern(const Token *start);

private:
    void quoteElogStringArgWith_SError(const Token *tok, const std::string& elogName);

    void getErrorMessages(ErrorLogger *errorLogger, const Settings *settings) const {
        CheckElog c(0, settings, errorLogger);
        c.quoteElogStringArgWith_SError(0, "ELOGElog");
    }

    static std::string myName() {
        return "Elog";
    }

    std::string classInfo() const {
        return "Check if the elog usage is correct.\n";
    }
};
/// @}
//---------------------------------------------------------------------------
#endif // checkelogH
