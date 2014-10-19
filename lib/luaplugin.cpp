//---------------------------------------------------------------------------
// lua checker plugin
//---------------------------------------------------------------------------

#include "luaplugin.h"
#include "symboldatabase.h"

extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
};
#include "lua_tinker.h"
#include "lua/fflua.h"
using namespace ff;
//---------------------------------------------------------------------------


// Register this check class (by creating a static instance of it)
namespace {
    LuaPlugin instance;
}

bool IsSameToken(Token* tok1, Token* tok2) {
    return tok1 == tok2;
}

void defToken(lua_State* L) {
    typedef Token* (Token::*GetTokenFunc)() const;
    typedef const Token* (Token::*GetConstTokenFunc)() const;
    typedef const Token* (Token::*GetTokenFuncInt)(int) const;
    typedef const Scope* (Token::*GetScopeFunc)() const;
    typedef const Function* (Token::*GetFunctionFunc)() const;
    typedef const Variable* (Token::*GetVariableFunc)() const;
    typedef Token::Type(Token::*GetTypeFunc)() const;
    typedef bool (Token::*GetBoolFunc)() const;
    typedef unsigned int (Token::*GetUIntFunc)() const;
    typedef const std::string& (Token::*GetStringFunc)() const;
    typedef void (Token::*printOut1)(const char *title) const;

    fflua_register_t<Token>(L, "Token")
        .def((GetStringFunc)&Token::str, "str")
        .def((GetTokenFuncInt)&Token::tokAt, "tokAt")
    .def((GetTokenFuncInt)&Token::linkAt, "linkAt")
    .def(&Token::strAt, "strAt")
    .def((GetTypeFunc)&Token::type, "type")
    .def((GetBoolFunc)&Token::isKeyword, "isKeyword")
    .def(&Token::isName, "isName")
    .def(&Token::isUpperCaseName, "isUpperCaseName")
    .def(&Token::isLiteral, "isLiteral")
    .def(&Token::isNumber, "isNumber")
    .def(&Token::isOp, "isOp")
    .def(&Token::isConstOp, "isConstOp")
    .def(&Token::isExtendedOp, "isExtendedOp")
    .def(&Token::isArithmeticalOp, "isArithmeticalOp")
    .def(&Token::isComparisonOp, "isComparisonOp")
    .def(&Token::isAssignmentOp, "isAssignmentOp")
    .def(&Token::isBoolean, "isBoolean")

    .def((GetBoolFunc)&Token::isUnsigned, "isUnsigned")
    .def((GetBoolFunc)&Token::isSigned, "isSigned")
    .def((GetBoolFunc)&Token::isPointerCompare, "isPointerCompare")
    .def((GetBoolFunc)&Token::isLong, "isLong")
    .def((GetBoolFunc)&Token::isStandardType, "isStandardType")
    .def((GetBoolFunc)&Token::isExpandedMacro, "isExpandedMacro")
    .def((GetBoolFunc)&Token::isAttributeConstructor, "isAttributeConstructor")
    .def((GetBoolFunc)&Token::isAttributeDestructor, "isAttributeDestructor")
    .def((GetBoolFunc)&Token::isAttributeUnused, "isAttributeUnused")
    .def((GetBoolFunc)&Token::isAttributeUsed, "isAttributeUsed")
    .def((GetBoolFunc)&Token::isAttributePure, "isAttributePure")
    .def((GetBoolFunc)&Token::isAttributeConst, "isAttributeConst")
    .def((GetBoolFunc)&Token::isAttributeNothrow, "isAttributeNothrow")
    .def((GetBoolFunc)&Token::isDeclspecNothrow, "isDeclspecNothrow")


    .def((GetUIntFunc)&Token::linenr, "linenr")
    .def((GetUIntFunc)&Token::fileIndex, "fileIndex")
    .def((GetTokenFunc)&Token::next, "next")
    .def((GetTokenFunc)&Token::previous, "previous")
    .def((GetUIntFunc)&Token::varId, "varId")
    .def((printOut1)&Token::printOut, "printOut")
    .def((GetTokenFunc)&Token::link, "link")
    .def((GetScopeFunc)&Token::scope, "scope")
    .def((GetFunctionFunc)&Token::function, "function")
    .def((GetVariableFunc)&Token::variable, "variable")
    .def(&Token::strValue, "strValue")
    .def(&Token::progressValue, "progressValue")
    .def(&Token::nextArgument, "nextArgument")
    .def((GetConstTokenFunc)&Token::findClosingBracket, "findClosingBracket")
    .def((GetStringFunc)&Token::originalName, "originalName")
    .def(&Token::getValue, "getValue")
    .def(&Token::getMaxValue, "getMaxValue")
    .def(&Token::getValueLE, "getValueLE")
    .def(&Token::getValueGE, "getValueGE")
    .def(&Token::getValueTokenMaxStrLength, "getValueTokenMaxStrLength")
    .def(&Token::getValueTokenMinStrSize, "getValueTokenMinStrSize")
    .def(&Token::getValueTokenDeadPointer, "getValueTokenDeadPointer");

    lua_tinker::table* tokenType = new lua_tinker::table(L, "TokenType");
    tokenType->set("eVariable", Token::eVariable);
    tokenType->set("eType", Token::eType);
    tokenType->set("eFunction", Token::eFunction);
    tokenType->set("eKeyword", Token::eKeyword);
    tokenType->set("eName", Token::eName);
    tokenType->set("eNumber", Token::eNumber);
    tokenType->set("eString", Token::eString);
    tokenType->set("eChar", Token::eChar);
    tokenType->set("eBoolean", Token::eBoolean);
    tokenType->set("eLiteral", Token::eLiteral);
    tokenType->set("eArithmeticalOp", Token::eArithmeticalOp);
    tokenType->set("eComparisonOp", Token::eComparisonOp);
    tokenType->set("eAssignmentOp", Token::eAssignmentOp);
    tokenType->set("eLogicalOp", Token::eLogicalOp);
    tokenType->set("eBitOp", Token::eBitOp);
    tokenType->set("eIncDecOp", Token::eIncDecOp);
    tokenType->set("eExtendedOp", Token::eExtendedOp);
    tokenType->set("eBracket", Token::eBracket);
    tokenType->set("eOther", Token::eOther);
    tokenType->set("eNone", Token::eNone);

    typedef Token *(*findsimplematch2)(Token *tok, const char pattern[]);
    typedef Token *(*findsimplematch3)(Token *tok, const char pattern[], const Token *end);
    typedef Token *(*findmatch3)(Token *tok, const char pattern[], unsigned int varId);
    typedef Token *(*findmatch4)(Token *tok, const char pattern[], const Token *end, unsigned int varId);
    //lua_tinker::def(L, "findsimplematch", (findsimplematch2)&Token::findsimplematch);
    lua_tinker::def(L, "findsimplematch", (findsimplematch3)&Token::findsimplematch);
    //lua_tinker::def(L, "findmatch", (findmatch3)&Token::findmatch);
    lua_tinker::def(L, "findmatch", (findmatch4)&Token::findmatch);
    lua_tinker::def(L, "multiCompare", &Token::multiCompare);

    lua_tinker::def(L, "IsSameToken", &IsSameToken);
    lua_tinker::def(L, "simpleMatch", &Token::simpleMatch);
    lua_tinker::def(L, "Match", &Token::Match);
    lua_tinker::def(L, "getStrLength", &Token::getStrLength);
    lua_tinker::def(L, "getStrSize", &Token::getStrSize);
    lua_tinker::def(L, "getCharAt", &Token::getCharAt);

}

void defTokenizer(lua_State* L)
{
    fflua_register_t<Tokenizer>(L, "Tokenizer")
    .def(&Tokenizer::isC, "isC")
    .def(&Tokenizer::isCPP, "isCPP")
    .def(&Tokenizer::tokens, "tokens");
}

void defSeverity(lua_State* L)
{
    lua_tinker::table* severityType = new lua_tinker::table(L, "SeverityType");
    severityType->set("none", Severity::none);
    severityType->set("error", Severity::error);
    severityType->set("warning", Severity::warning);
    severityType->set("style", Severity::style);
    severityType->set("performance", Severity::performance);
    severityType->set("portability", Severity::portability);
    severityType->set("information", Severity::information);
    severityType->set("debug", Severity::debug);
}

void defScope(lua_State* L)
{
    typedef const Scope *(Scope::*GetScopeFunc)(const std::string&) const;
    typedef const Type *(Scope::*GetTypeFunc)(const std::string&) const;

    fflua_register_t<Scope>(L, "Scope")
    .def(&Scope::check, "check")
    .def(&Scope::className, "className")
    .def(&Scope::classDef, "classDef")
    .def(&Scope::classStart, "classStart")
    .def(&Scope::classEnd, "classEnd")
    .def(&Scope::functionList, "functionList")
    .def(&Scope::varlist, "varlist")
    .def(&Scope::nestedIn, "nestedIn")
    .def(&Scope::nestedList, "nestedList")
    .def(&Scope::numConstructors, "numConstructors")
    .def(&Scope::numCopyOrMoveConstructors, "numCopyOrMoveConstructors")
    .def(&Scope::usingList, "usingList")
    .def(&Scope::type, "type")
    .def(&Scope::definedType, "definedType")
    .def(&Scope::definedTypes, "definedTypes")
    .def(&Scope::functionOf, "functionOf")
    .def(&Scope::function, "function")

    .def(&Scope::isClassOrStruct, "isClassOrStruct")
    .def(&Scope::isExecutable, "isExecutable")
    .def(&Scope::isLocal, "isLocal")
    .def(&Scope::findFunction, "findFunction")
    .def(&Scope::findInNestedList, "findInNestedList")
    .def((GetScopeFunc)&Scope::findRecordInNestedList, "findRecordInNestedList")
    .def((GetTypeFunc)&Scope::findType, "findType")
    .def(&Scope::findInNestedListRecursive, "findInNestedListRecursive")
    .def(&Scope::getVariableList, "getVariableList")
    .def(&Scope::getDestructor, "getDestructor")
    .def(&Scope::getNestedNonFunctions, "getNestedNonFunctions")
    .def(&Scope::hasDefaultConstructor, "hasDefaultConstructor")
    .def(&Scope::defaultAccess, "defaultAccess")
    .def(&Scope::checkVariable, "checkVariable")
    .def(&Scope::getVariable, "getVariable");

    lua_tinker::table* sopeType = new lua_tinker::table(L, "ScopeType");
    sopeType->set("eGlobal", Scope::eGlobal);
    sopeType->set("eClass", Scope::eClass);
    sopeType->set("eStruct", Scope::eStruct);
    sopeType->set("eUnion", Scope::eUnion);
    sopeType->set("eNamespace", Scope::eNamespace);
    sopeType->set("eFunction", Scope::eFunction);
    sopeType->set("eIf", Scope::eIf);
    sopeType->set("eElse", Scope::eElse);
    sopeType->set("eFor", Scope::eFor);
    sopeType->set("eWhile", Scope::eWhile);
    sopeType->set("eDo", Scope::eDo);
    sopeType->set("eSwitch", Scope::eSwitch);
    sopeType->set("eUnconditional", Scope::eUnconditional);
    sopeType->set("eTry", Scope::eTry);
    sopeType->set("eCatch", Scope::eCatch);
    sopeType->set("eLambda", Scope::eLambda);
}

void defType(lua_State* L)
{
    fflua_register_t<Type>(L, "Type")
    .def(&Type::classDef, "classDef")
    .def(&Type::classScope, "classScope")
    .def(&Type::enclosingScope, "enclosingScope")
    .def(&Type::derivedFrom, "derivedFrom")
    .def(&Type::friendList, "friendList")

    .def(&Type::name, "name")
    .def(&Type::initBaseInfo, "initBaseInfo")
    .def(&Type::getFunction, "getFunction")
    .def(&Type::hasCircularDependencies, "hasCircularDependencies");
}

void defFunction(lua_State* L)
{
    fflua_register_t<Function>(L, "Function")

    .def(&Function::tokenDef, "tokenDef")
    .def(&Function::argDef, "argDef")
    .def(&Function::token, "token")
    .def(&Function::arg, "arg")
    .def(&Function::retDef, "retDef")
    .def(&Function::retType, "retType")
    .def(&Function::functionScope, "functionScope")
    .def(&Function::nestedIn, "nestedIn")
    .def(&Function::argumentList, "argumentList")
    .def(&Function::initArgCount, "initArgCount")
    .def(&Function::type, "type")
    .def(&Function::access, "access")
    .def(&Function::hasBody, "hasBody")
    .def(&Function::isInline, "isInline")
    .def(&Function::isConst, "isConst")
    .def(&Function::isVirtual, "isVirtual")
    .def(&Function::isPure, "isPure")
    .def(&Function::isStatic, "isStatic")
    .def(&Function::isFriend, "isFriend")
    .def(&Function::isExplicit, "isExplicit")
    .def(&Function::isDefault, "isDefault")
    .def(&Function::isDelete, "isDelete")
    .def(&Function::isNoExcept, "isNoExcept")
    .def(&Function::isThrow, "isThrow")
    .def(&Function::isOperator, "isOperator")
    .def(&Function::noexceptArg, "noexceptArg")
    .def(&Function::throwArg, "throwArg")

    .def(&Function::name, "name")
    .def(&Function::argCount, "argCount")
    .def(&Function::minArgCount, "minArgCount")
    .def(&Function::getArgumentVar, "getArgumentVar")
    .def(&Function::initializedArgCount, "initializedArgCount")
    .def(&Function::addArguments, "addArguments")
    .def(&Function::isImplicitlyVirtual, "isImplicitlyVirtual")
    .def(&Function::isConstructor, "isConstructor")
    .def(&Function::isDestructor, "isDestructor")
    .def(&Function::isAttributeConstructor, "isAttributeConstructor")
    .def(&Function::isAttributeDestructor, "isAttributeDestructor")
    .def(&Function::isAttributePure, "isAttributePure")
    .def(&Function::isAttributeConst, "isAttributeConst")
    .def(&Function::isAttributeNothrow, "isAttributeNothrow")
    .def(&Function::isDeclspecNothrow, "isDeclspecNothrow");

    lua_tinker::table* functionType = new lua_tinker::table(L, "FunctionType");
    functionType->set("eConstructor", Function::eConstructor);
    functionType->set("eCopyConstructor", Function::eCopyConstructor);
    functionType->set("eMoveConstructor", Function::eMoveConstructor);
    functionType->set("eOperatorEqual", Function::eOperatorEqual);
    functionType->set("eDestructor", Function::eDestructor);
    functionType->set("eFunction", Function::eFunction);
}

void defVariable(lua_State* L)
{
    typedef const Type *(Variable::*GetTypeFunc)() const;
    typedef bool(Variable::*GetBoolFunc)() const;

    fflua_register_t<Variable>(L, "Variable")
    .def(&Variable::nameToken, "nameToken")
    .def(&Variable::typeStartToken, "typeStartToken")
    .def(&Variable::typeEndToken, "typeEndToken")
    .def(&Variable::name, "name")
    .def(&Variable::declarationId, "declarationId")
    .def(&Variable::index, "index")
    .def(&Variable::isPublic, "isPublic")
    .def(&Variable::isProtected, "isProtected")
    .def(&Variable::isPrivate, "isPrivate")
    .def(&Variable::isGlobal, "isGlobal")
    .def(&Variable::isNamespace, "isNamespace")
    .def(&Variable::isArgument, "isArgument")
    .def(&Variable::isLocal, "isLocal")
    .def(&Variable::isMutable, "isMutable")
    .def(&Variable::isStatic, "isStatic")
    .def(&Variable::isExtern, "isExtern")
    .def(&Variable::isConst, "isConst")
    .def(&Variable::isThrow, "isThrow")
    .def(&Variable::isClass, "isClass")
    .def(&Variable::isArray, "isArray")
    .def(&Variable::isPointer, "isPointer")
    .def(&Variable::isArrayOrPointer, "isArrayOrPointer")
    .def(&Variable::isReference, "isReference")
    .def(&Variable::isRValueReference, "isRValueReference")
    .def(&Variable::hasDefault, "hasDefault")
    .def((GetTypeFunc)&Variable::type, "type")
    .def(&Variable::typeScope, "typeScope")
    .def(&Variable::scope, "scope")
    .def(&Variable::dimensions, "dimensions")
    .def(&Variable::dimension, "dimension")
    .def(&Variable::dimensionKnown, "dimensionKnown")
    .def((GetBoolFunc)&Variable::isStlType, "isStlType")
    .def(&Variable::isStlStringType, "isStlStringType")
    .def(&Variable::isFloatingType, "isFloatingType")
    .def(&Variable::isIntegralType, "isIntegralType");
}

void defSymbolDatabase(lua_State* L)
{
    typedef const Type *(SymbolDatabase::*GetTypeFunc)(const Token *, const Scope *) const;
    typedef const Scope *(SymbolDatabase::*GetScopeFunc)(const Token *, const Scope *) const;

    fflua_register_t<SymbolDatabase>(L, "SymbolDatabase")
    .def(&SymbolDatabase::scopeList, "scopeList")
    .def(&SymbolDatabase::functionScopes, "functionScopes")
    .def(&SymbolDatabase::classAndStructScopes, "classAndStructScopes")
    .def(&SymbolDatabase::typeList, "typeList")

    .def(&SymbolDatabase::findVariableType, "findVariableType")
    .def(&SymbolDatabase::findFunction, "findFunction")
    .def(&SymbolDatabase::findScopeByName, "findScopeByName")
    .def((GetTypeFunc)&SymbolDatabase::findType, "findType")
    .def((GetScopeFunc)&SymbolDatabase::findScope, "findScope")
    .def(&SymbolDatabase::isClassOrStruct, "isClassOrStruct")
    .def(&SymbolDatabase::getVariableFromVarId, "getVariableFromVarId")
    .def(&SymbolDatabase::getVariableListSize, "getVariableListSize")
    .def(&SymbolDatabase::debugMessage, "debugMessage")
    .def(&SymbolDatabase::printOut, "printOut")
    .def(&SymbolDatabase::printVariable, "printVariable")
    .def(&SymbolDatabase::printXml, "printXml");
}

void LuaPlugin::quoteElogStringArg()
{
    lua_State* L = lua_open();
    luaopen_base(L);
    luaopen_string(L);

    defToken(L);
    defTokenizer(L);
    defSeverity(L);
    defScope(L);
    defType(L);
    defFunction(L);
    defVariable(L);
    defSymbolDatabase(L);

    lua_tinker::class_add<AccessControl>(L, "AccessControl");
    lua_tinker::class_add<ValueFlow::Value>(L, "ValueFlow.Value");

    lua_tinker::class_add<LuaPlugin>(L, "LuaPlugin");
    lua_tinker::class_def<LuaPlugin>(L, "reportError", &LuaPlugin::luaReportError);

    lua_tinker::set(L, "_tokenizer", this->_tokenizer);
    lua_tinker::set(L, "_checkPlugin", this);

    lua_tinker::dostring(L, "print(\"hello\")");

    lua_tinker::dostring(L, "print(\"isC: \" .. tostring(_tokenizer:isC()))");
    lua_tinker::dostring(L, "print(\"isCPP: \" .. tostring(_tokenizer:isCPP()))");

    lua_tinker::dostring(L, "print(\"str: \" .. _tokenizer:tokens():str())");
    lua_tinker::dostring(L, "print(\"Match: \" .. tostring(Match(_tokenizer:tokens(), \"void\")))");
    lua_tinker::dostring(L, "print(_tokenizer:tokens():type())");
    lua_tinker::dostring(L, "print(\"Token::eFunction: \" .. TokenType.eFunction)");

    lua_tinker::dofile(L, "E:\\lbn\\cppcheck\\bin\\ChekElog.lua");

    lua_close(L);
    return;
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


void LuaPlugin::quoteElogStringArgWith_SError(const Token *tok, const std::string& elogName)
{
    std::string literal_str = tok ? tok->str() : "null";
    reportError(tok, Severity::error,
        "quoteElogStringArgWith_S",
        "You should quote literal string argument  " + literal_str + "  of '" + elogName + "' with macro _S().\n"
        "You should quote literal string argument  " + literal_str + "  of '" + elogName + "' with macro _S().");
}

const Token* LuaPlugin::findElogPattern(const Token* start)
{
    return Token::findmatch(start, "ELOGElog|elog_finish|ELOGSetLastError|ELOGSetLastError_ ( %any%");
}

void LuaPlugin::luaReportError(const Token *tok, const Severity::SeverityType severity, const char* id, const char* msg, bool inconclusive) {
    reportError(tok, severity, id, msg, inconclusive);
}
