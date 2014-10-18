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

    lua_tinker::class_add<Token>(L, "Token");
    lua_tinker::class_def<Token>(L, "str", &Token::str);
    lua_tinker::class_def<Token>(L, "tokAt", (GetTokenFuncInt)&Token::tokAt);
    lua_tinker::class_def<Token>(L, "linkAt", (GetTokenFuncInt)&Token::linkAt);
    lua_tinker::class_def<Token>(L, "strAt", &Token::strAt);

    lua_tinker::def(L, "simpleMatch", &Token::simpleMatch);
    lua_tinker::def(L, "Match", &Token::Match);
    lua_tinker::def(L, "getStrLength", &Token::getStrLength);
    lua_tinker::def(L, "getStrSize", &Token::getStrSize);
    lua_tinker::def(L, "getCharAt", &Token::getCharAt);

    lua_tinker::class_def<Token>(L, "type", (GetTypeFunc)&Token::type);
    lua_tinker::class_def<Token>(L, "isKeyword", (GetBoolFunc)&Token::isKeyword);
    lua_tinker::class_def<Token>(L, "isName", &Token::isName);
    lua_tinker::class_def<Token>(L, "isUpperCaseName", &Token::isUpperCaseName);
    lua_tinker::class_def<Token>(L, "isLiteral", &Token::isLiteral);
    lua_tinker::class_def<Token>(L, "isNumber", &Token::isNumber);
    lua_tinker::class_def<Token>(L, "isOp", &Token::isOp);
    lua_tinker::class_def<Token>(L, "isConstOp", &Token::isConstOp);
    lua_tinker::class_def<Token>(L, "isExtendedOp", &Token::isExtendedOp);
    lua_tinker::class_def<Token>(L, "isArithmeticalOp", &Token::isArithmeticalOp);
    lua_tinker::class_def<Token>(L, "isComparisonOp", &Token::isComparisonOp);
    lua_tinker::class_def<Token>(L, "isAssignmentOp", &Token::isAssignmentOp);
    lua_tinker::class_def<Token>(L, "isBoolean", &Token::isBoolean);

    lua_tinker::class_def<Token>(L, "isUnsigned", (GetBoolFunc)&Token::isUnsigned);
    lua_tinker::class_def<Token>(L, "isSigned", (GetBoolFunc)&Token::isSigned);
    lua_tinker::class_def<Token>(L, "isPointerCompare", (GetBoolFunc)&Token::isPointerCompare);
    lua_tinker::class_def<Token>(L, "isLong", (GetBoolFunc)&Token::isLong);
    lua_tinker::class_def<Token>(L, "isStandardType", (GetBoolFunc)&Token::isStandardType);
    lua_tinker::class_def<Token>(L, "isExpandedMacro", (GetBoolFunc)&Token::isExpandedMacro);
    lua_tinker::class_def<Token>(L, "isAttributeConstructor", (GetBoolFunc)&Token::isAttributeConstructor);
    lua_tinker::class_def<Token>(L, "isAttributeDestructor", (GetBoolFunc)&Token::isAttributeDestructor);
    lua_tinker::class_def<Token>(L, "isAttributeUnused", (GetBoolFunc)&Token::isAttributeUnused);
    lua_tinker::class_def<Token>(L, "isAttributeUsed", (GetBoolFunc)&Token::isAttributeUsed);
    lua_tinker::class_def<Token>(L, "isAttributePure", (GetBoolFunc)&Token::isAttributePure);
    lua_tinker::class_def<Token>(L, "isAttributeConst", (GetBoolFunc)&Token::isAttributeConst);
    lua_tinker::class_def<Token>(L, "isAttributeNothrow", (GetBoolFunc)&Token::isAttributeNothrow);
    lua_tinker::class_def<Token>(L, "isDeclspecNothrow", (GetBoolFunc)&Token::isDeclspecNothrow);

    typedef Token *(*findsimplematch2)(Token *tok, const char pattern[]);
    typedef Token *(*findsimplematch3)(Token *tok, const char pattern[], const Token *end);
    typedef Token *(*findmatch3)(Token *tok, const char pattern[], unsigned int varId);
    typedef Token *(*findmatch4)(Token *tok, const char pattern[], const Token *end, unsigned int varId);
    //lua_tinker::def(L, "findsimplematch", (findsimplematch2)&Token::findsimplematch);
    lua_tinker::def(L, "findsimplematch", (findsimplematch3)&Token::findsimplematch);
    //lua_tinker::def(L, "findmatch", (findmatch3)&Token::findmatch);
    lua_tinker::def(L, "findmatch", (findmatch4)&Token::findmatch);
    lua_tinker::def(L, "multiCompare", &Token::multiCompare);

    lua_tinker::class_def<Token>(L, "linenr", (GetUIntFunc)&Token::linenr);
    lua_tinker::class_def<Token>(L, "fileIndex", (GetUIntFunc)&Token::fileIndex);
    lua_tinker::class_def<Token>(L, "next", (GetTokenFunc)&Token::next);
    lua_tinker::class_def<Token>(L, "previous", (GetTokenFunc)&Token::previous);
    lua_tinker::class_def<Token>(L, "varId", (GetUIntFunc)&Token::varId);
    typedef void (Token::*printOut1)(const char *title) const;
    lua_tinker::class_def<Token>(L, "printOut", (printOut1)&Token::printOut);
    lua_tinker::class_def<Token>(L, "link", (GetTokenFunc)&Token::link);
    lua_tinker::class_def<Token>(L, "scope", (GetScopeFunc)&Token::scope);
    lua_tinker::class_def<Token>(L, "function", (GetFunctionFunc)&Token::function);
    lua_tinker::class_def<Token>(L, "variable", (GetVariableFunc)&Token::variable);
    lua_tinker::class_def<Token>(L, "strValue", &Token::strValue);
    lua_tinker::class_def<Token>(L, "progressValue", &Token::progressValue);
    lua_tinker::class_def<Token>(L, "nextArgument", &Token::nextArgument);
    lua_tinker::class_def<Token>(L, "findClosingBracket", (GetConstTokenFunc)&Token::findClosingBracket);
    lua_tinker::class_def<Token>(L, "originalName", &Token::originalName);
    lua_tinker::class_def<Token>(L, "getValue", &Token::getValue);
    lua_tinker::class_def<Token>(L, "getMaxValue", &Token::getMaxValue);
    lua_tinker::class_def<Token>(L, "getValueLE", &Token::getValueLE);
    lua_tinker::class_def<Token>(L, "getValueGE", &Token::getValueGE);
    lua_tinker::class_def<Token>(L, "getValueTokenMaxStrLength", &Token::getValueTokenMaxStrLength);
    lua_tinker::class_def<Token>(L, "getValueTokenMinStrSize", &Token::getValueTokenMinStrSize);
    lua_tinker::class_def<Token>(L, "getValueTokenDeadPointer", &Token::getValueTokenDeadPointer);

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

    lua_tinker::def(L, "IsSameToken", &IsSameToken);
}

void defTokenizer(lua_State* L)
{
    lua_tinker::class_add<Tokenizer>(L, "Tokenizer");
    lua_tinker::class_def<Tokenizer>(L, "isC", &Tokenizer::isC);
    lua_tinker::class_def<Tokenizer>(L, "isCPP", &Tokenizer::isCPP);
    lua_tinker::class_def<Tokenizer>(L, "tokens", &Tokenizer::tokens);
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

    lua_tinker::class_add<Scope>(L, "Scope");
    lua_tinker::class_mem<Scope>(L, "check", &Scope::check);
    lua_tinker::class_mem<Scope>(L, "className", &Scope::className);
    lua_tinker::class_mem<Scope>(L, "classDef", &Scope::classDef);
    lua_tinker::class_mem<Scope>(L, "classStart", &Scope::classStart);
    lua_tinker::class_mem<Scope>(L, "classEnd", &Scope::classEnd);
    lua_tinker::class_mem<Scope>(L, "functionList", &Scope::functionList);
    lua_tinker::class_mem<Scope>(L, "varlist", &Scope::varlist);
    lua_tinker::class_mem<Scope>(L, "nestedIn", &Scope::nestedIn);
    lua_tinker::class_mem<Scope>(L, "nestedList", &Scope::nestedList);
    lua_tinker::class_mem<Scope>(L, "numConstructors", &Scope::numConstructors);
    lua_tinker::class_mem<Scope>(L, "numCopyOrMoveConstructors", &Scope::numCopyOrMoveConstructors);
    lua_tinker::class_mem<Scope>(L, "usingList", &Scope::usingList);
    lua_tinker::class_mem<Scope>(L, "type", &Scope::type);
    lua_tinker::class_mem<Scope>(L, "definedType", &Scope::definedType);
    lua_tinker::class_mem<Scope>(L, "definedTypes", &Scope::definedTypes);
    lua_tinker::class_mem<Scope>(L, "functionOf", &Scope::functionOf);
    lua_tinker::class_mem<Scope>(L, "function", &Scope::function);

    lua_tinker::class_def<Scope>(L, "isClassOrStruct", &Scope::isClassOrStruct);
    lua_tinker::class_def<Scope>(L, "isExecutable", &Scope::isExecutable);
    lua_tinker::class_def<Scope>(L, "isLocal", &Scope::isLocal);
    lua_tinker::class_def<Scope>(L, "findFunction", &Scope::findFunction);
    lua_tinker::class_def<Scope>(L, "findInNestedList", &Scope::findInNestedList);
    lua_tinker::class_def<Scope>(L, "findRecordInNestedList", (GetScopeFunc)&Scope::findRecordInNestedList);
    lua_tinker::class_def<Scope>(L, "findType", (GetTypeFunc)&Scope::findType);
    lua_tinker::class_def<Scope>(L, "findInNestedListRecursive", &Scope::findInNestedListRecursive);
    lua_tinker::class_def<Scope>(L, "getVariableList", &Scope::getVariableList);
    lua_tinker::class_def<Scope>(L, "getDestructor", &Scope::getDestructor);
    lua_tinker::class_def<Scope>(L, "getNestedNonFunctions", &Scope::getNestedNonFunctions);
    lua_tinker::class_def<Scope>(L, "hasDefaultConstructor", &Scope::hasDefaultConstructor);
    lua_tinker::class_def<Scope>(L, "defaultAccess", &Scope::defaultAccess);
    lua_tinker::class_def<Scope>(L, "checkVariable", &Scope::checkVariable);
    lua_tinker::class_def<Scope>(L, "getVariable", &Scope::getVariable);

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
    lua_tinker::class_add<Type>(L, "Type");
    lua_tinker::class_mem<Type>(L, "classDef", &Type::classDef);
    lua_tinker::class_mem<Type>(L, "classScope", &Type::classScope);
    lua_tinker::class_mem<Type>(L, "enclosingScope", &Type::enclosingScope);
    lua_tinker::class_mem<Type>(L, "derivedFrom", &Type::derivedFrom);
    lua_tinker::class_mem<Type>(L, "friendList", &Type::friendList);

    lua_tinker::class_def<Type>(L, "name", &Type::name);
    lua_tinker::class_def<Type>(L, "initBaseInfo", &Type::initBaseInfo);
    lua_tinker::class_def<Type>(L, "getFunction", &Type::getFunction);
    lua_tinker::class_def<Type>(L, "hasCircularDependencies", &Type::hasCircularDependencies);
}

void defFunction(lua_State* L)
{
    lua_tinker::class_add<Function>(L, "Function");

    lua_tinker::class_mem<Function>(L, "tokenDef", &Function::tokenDef);
    lua_tinker::class_mem<Function>(L, "argDef", &Function::argDef);
    lua_tinker::class_mem<Function>(L, "token", &Function::token);
    lua_tinker::class_mem<Function>(L, "arg", &Function::arg);
    lua_tinker::class_mem<Function>(L, "retDef", &Function::retDef);
    lua_tinker::class_mem<Function>(L, "retType", &Function::retType);
    lua_tinker::class_mem<Function>(L, "functionScope", &Function::functionScope);
    lua_tinker::class_mem<Function>(L, "nestedIn", &Function::nestedIn);
    lua_tinker::class_mem<Function>(L, "argumentList", &Function::argumentList);
    lua_tinker::class_mem<Function>(L, "initArgCount", &Function::initArgCount);
    lua_tinker::class_mem<Function>(L, "type", &Function::type);
    lua_tinker::class_mem<Function>(L, "access", &Function::access);
    lua_tinker::class_mem<Function>(L, "hasBody", &Function::hasBody);
    lua_tinker::class_mem<Function>(L, "isInline", &Function::isInline);
    lua_tinker::class_mem<Function>(L, "isConst", &Function::isConst);
    lua_tinker::class_mem<Function>(L, "isVirtual", &Function::isVirtual);
    lua_tinker::class_mem<Function>(L, "isPure", &Function::isPure);
    lua_tinker::class_mem<Function>(L, "isStatic", &Function::isStatic);
    lua_tinker::class_mem<Function>(L, "isFriend", &Function::isFriend);
    lua_tinker::class_mem<Function>(L, "isExplicit", &Function::isExplicit);
    lua_tinker::class_mem<Function>(L, "isDefault", &Function::isDefault);
    lua_tinker::class_mem<Function>(L, "isDelete", &Function::isDelete);
    lua_tinker::class_mem<Function>(L, "isNoExcept", &Function::isNoExcept);
    lua_tinker::class_mem<Function>(L, "isThrow", &Function::isThrow);
    lua_tinker::class_mem<Function>(L, "isOperator", &Function::isOperator);
    lua_tinker::class_mem<Function>(L, "noexceptArg", &Function::noexceptArg);
    lua_tinker::class_mem<Function>(L, "throwArg", &Function::throwArg);

    lua_tinker::class_def<Function>(L, "name", &Function::name);
    lua_tinker::class_def<Function>(L, "argCount", &Function::argCount);
    lua_tinker::class_def<Function>(L, "minArgCount", &Function::minArgCount);
    lua_tinker::class_def<Function>(L, "getArgumentVar", &Function::getArgumentVar);
    lua_tinker::class_def<Function>(L, "initializedArgCount", &Function::initializedArgCount);
    lua_tinker::class_def<Function>(L, "addArguments", &Function::addArguments);
    lua_tinker::class_def<Function>(L, "isImplicitlyVirtual", &Function::isImplicitlyVirtual);
    lua_tinker::class_def<Function>(L, "isConstructor", &Function::isConstructor);
    lua_tinker::class_def<Function>(L, "isDestructor", &Function::isDestructor);
    lua_tinker::class_def<Function>(L, "isAttributeConstructor", &Function::isAttributeConstructor);
    lua_tinker::class_def<Function>(L, "isAttributeDestructor", &Function::isAttributeDestructor);
    lua_tinker::class_def<Function>(L, "isAttributePure", &Function::isAttributePure);
    lua_tinker::class_def<Function>(L, "isAttributeConst", &Function::isAttributeConst);
    lua_tinker::class_def<Function>(L, "isAttributeNothrow", &Function::isAttributeNothrow);
    lua_tinker::class_def<Function>(L, "isDeclspecNothrow", &Function::isDeclspecNothrow);

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
    lua_tinker::class_add<Variable>(L, "Variable");
    
    typedef const Type *(Variable::*GetTypeFunc)() const;

    lua_tinker::class_def<Variable>(L, "nameToken", &Variable::nameToken);
    lua_tinker::class_def<Variable>(L, "typeStartToken", &Variable::typeStartToken);
    lua_tinker::class_def<Variable>(L, "typeEndToken", &Variable::typeEndToken);
    lua_tinker::class_def<Variable>(L, "name", &Variable::name);
    lua_tinker::class_def<Variable>(L, "declarationId", &Variable::declarationId);
    lua_tinker::class_def<Variable>(L, "index", &Variable::index);
    lua_tinker::class_def<Variable>(L, "isPublic", &Variable::isPublic);
    lua_tinker::class_def<Variable>(L, "isProtected", &Variable::isProtected);
    lua_tinker::class_def<Variable>(L, "isPrivate", &Variable::isPrivate);
    lua_tinker::class_def<Variable>(L, "isGlobal", &Variable::isGlobal);
    lua_tinker::class_def<Variable>(L, "isNamespace", &Variable::isNamespace);
    lua_tinker::class_def<Variable>(L, "isArgument", &Variable::isArgument);
    lua_tinker::class_def<Variable>(L, "isLocal", &Variable::isLocal);
    lua_tinker::class_def<Variable>(L, "isMutable", &Variable::isMutable);
    lua_tinker::class_def<Variable>(L, "isStatic", &Variable::isStatic);
    lua_tinker::class_def<Variable>(L, "isExtern", &Variable::isExtern);
    lua_tinker::class_def<Variable>(L, "isConst", &Variable::isConst);
    lua_tinker::class_def<Variable>(L, "isThrow", &Variable::isThrow);
    lua_tinker::class_def<Variable>(L, "isClass", &Variable::isClass);
    lua_tinker::class_def<Variable>(L, "isArray", &Variable::isArray);
    lua_tinker::class_def<Variable>(L, "isPointer", &Variable::isPointer);
    lua_tinker::class_def<Variable>(L, "isArrayOrPointer", &Variable::isArrayOrPointer);
    lua_tinker::class_def<Variable>(L, "isReference", &Variable::isReference);
    lua_tinker::class_def<Variable>(L, "isRValueReference", &Variable::isRValueReference);
    lua_tinker::class_def<Variable>(L, "hasDefault", &Variable::hasDefault);
    lua_tinker::class_def<Variable>(L, "type", (GetTypeFunc)&Variable::type);
    lua_tinker::class_def<Variable>(L, "typeScope", &Variable::typeScope);
    lua_tinker::class_def<Variable>(L, "scope", &Variable::scope);
    lua_tinker::class_def<Variable>(L, "dimensions", &Variable::dimensions);
    lua_tinker::class_def<Variable>(L, "dimension", &Variable::dimension);
    lua_tinker::class_def<Variable>(L, "dimensionKnown", &Variable::dimensionKnown);
    lua_tinker::class_def<Variable>(L, "isStlType", &Variable::isStlType);
    lua_tinker::class_def<Variable>(L, "isStlStringType", &Variable::isStlStringType);
    lua_tinker::class_def<Variable>(L, "isFloatingType", &Variable::isFloatingType);
    lua_tinker::class_def<Variable>(L, "isIntegralType", &Variable::isIntegralType);
}

void defSymbolDatabase(lua_State* L)
{
    lua_tinker::class_add<SymbolDatabase>(L, "SymbolDatabase");

    typedef const Type *(SymbolDatabase::*GetTypeFunc)(const Token *, const Scope *) const;
    typedef const Scope *(SymbolDatabase::*GetScopeFunc)(const Token *, const Scope *) const;

    lua_tinker::class_mem<SymbolDatabase>(L, "scopeList", &SymbolDatabase::scopeList);
    lua_tinker::class_mem<SymbolDatabase>(L, "functionScopes", &SymbolDatabase::functionScopes);
    lua_tinker::class_mem<SymbolDatabase>(L, "classAndStructScopes", &SymbolDatabase::classAndStructScopes);
    lua_tinker::class_mem<SymbolDatabase>(L, "typeList", &SymbolDatabase::typeList);

    lua_tinker::class_def<SymbolDatabase>(L, "findVariableType", &SymbolDatabase::findVariableType);
    lua_tinker::class_def<SymbolDatabase>(L, "findFunction", &SymbolDatabase::findFunction);
    lua_tinker::class_def<SymbolDatabase>(L, "findScopeByName", &SymbolDatabase::findScopeByName);
    lua_tinker::class_def<SymbolDatabase>(L, "findType", (GetTypeFunc)&SymbolDatabase::findType);
    lua_tinker::class_def<SymbolDatabase>(L, "findScope", (GetScopeFunc)&SymbolDatabase::findScope);
    lua_tinker::class_def<SymbolDatabase>(L, "isClassOrStruct", &SymbolDatabase::isClassOrStruct);
    lua_tinker::class_def<SymbolDatabase>(L, "getVariableFromVarId", &SymbolDatabase::getVariableFromVarId);
    lua_tinker::class_def<SymbolDatabase>(L, "getVariableListSize", &SymbolDatabase::getVariableListSize);
    lua_tinker::class_def<SymbolDatabase>(L, "debugMessage", &SymbolDatabase::debugMessage);
    lua_tinker::class_def<SymbolDatabase>(L, "printOut", &SymbolDatabase::printOut);
    lua_tinker::class_def<SymbolDatabase>(L, "printVariable", &SymbolDatabase::printVariable);
    lua_tinker::class_def<SymbolDatabase>(L, "printXml", &SymbolDatabase::printXml);
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
