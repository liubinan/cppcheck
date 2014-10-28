//---------------------------------------------------------------------------
// lua checker plugin
//---------------------------------------------------------------------------

#include <typeinfo>

#include "luaplugin.h"
#include "symboldatabase.h"
#include "path.h"
#include "filelister.h"

extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
};
//#include "lua_tinker.h"
#include "lua/fflua.h"
using namespace ff;
//---------------------------------------------------------------------------

std::map<std::string, Check*> lua_checkers;

// Register this check class (by creating a static instance of it)
namespace {
    struct init_lua_plugin_t {
        init_lua_plugin_t() {
            std::string exe_dir = Path::getPathFromFilename(Path::getModuleFileName());
            std::string lua_plugin_dir = exe_dir + "/checkers";
            std::map<std::string, std::size_t> files;
            std::set<std::string> lua_extra;
            lua_extra.insert(".lua");

            FileLister::recursiveAddFiles(files, lua_plugin_dir, lua_extra);

            for (auto f : files)
            {
				fflua_t fflua;

                std::string lua_file = f.first;
                if (Path::getFilenameExtensionInLowerCase(lua_file) != ".lua") {
                    continue;
                }
				fflua.add_package_path(lua_plugin_dir);
                fflua.load_file(lua_file);

                if (fflua.is_function_exists("myName"))
                {
                    std::string check_name = fflua.call<string>("myName");
                    std::cout << "myName: " << check_name << std::endl;

                    bool simple_check_exists = fflua.is_function_exists("runSimplifiedChecks");
                    std::cout << "runSimplifiedChecks: " << simple_check_exists << std::endl;

                    bool check_exists = fflua.is_function_exists("runChecks");
                    std::cout << "runChecks: " << check_exists << std::endl;

					lua_checkers[check_name] = new LuaPlugin(check_name, lua_file, check_exists, simple_check_exists);
                }
            }

        }
    }init_lua_plugin;

}

LuaPlugin::LuaPlugin() : Check(myName()) {
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

    fflua_register_t<Token, int()> token_reg(L, "Token");
    token_reg.def((GetStringFunc)&Token::str, "str");
    token_reg.def((GetTokenFuncInt)&Token::tokAt, "tokAt");
    token_reg.def((GetTokenFuncInt)&Token::linkAt, "linkAt");
    token_reg.def(&Token::strAt, "strAt");
    token_reg.def((GetTypeFunc)&Token::type, "type");
    token_reg.def((GetBoolFunc)&Token::isKeyword, "isKeyword");
    token_reg.def(&Token::isName, "isName");
    token_reg.def(&Token::isUpperCaseName, "isUpperCaseName");
    token_reg.def(&Token::isLiteral, "isLiteral");
    token_reg.def(&Token::isNumber, "isNumber");
    token_reg.def(&Token::isOp, "isOp");
    token_reg.def(&Token::isConstOp, "isConstOp");
    token_reg.def(&Token::isExtendedOp, "isExtendedOp");
    token_reg.def(&Token::isArithmeticalOp, "isArithmeticalOp");
    token_reg.def(&Token::isComparisonOp, "isComparisonOp");
    token_reg.def(&Token::isAssignmentOp, "isAssignmentOp");
    token_reg.def(&Token::isBoolean, "isBoolean");

    token_reg.def((GetBoolFunc)&Token::isUnsigned, "isUnsigned");
    token_reg.def((GetBoolFunc)&Token::isSigned, "isSigned");
    token_reg.def((GetBoolFunc)&Token::isPointerCompare, "isPointerCompare");
    token_reg.def((GetBoolFunc)&Token::isLong, "isLong");
    token_reg.def((GetBoolFunc)&Token::isStandardType, "isStandardType");
    token_reg.def((GetBoolFunc)&Token::isExpandedMacro, "isExpandedMacro");
    token_reg.def((GetBoolFunc)&Token::isAttributeConstructor, "isAttributeConstructor");
    token_reg.def((GetBoolFunc)&Token::isAttributeDestructor, "isAttributeDestructor");
    token_reg.def((GetBoolFunc)&Token::isAttributeUnused, "isAttributeUnused");
    token_reg.def((GetBoolFunc)&Token::isAttributeUsed, "isAttributeUsed");
    token_reg.def((GetBoolFunc)&Token::isAttributePure, "isAttributePure");
    token_reg.def((GetBoolFunc)&Token::isAttributeConst, "isAttributeConst");
    token_reg.def((GetBoolFunc)&Token::isAttributeNothrow, "isAttributeNothrow");
    token_reg.def((GetBoolFunc)&Token::isDeclspecNothrow, "isDeclspecNothrow");


    token_reg.def((GetUIntFunc)&Token::linenr, "linenr");
    token_reg.def((GetUIntFunc)&Token::fileIndex, "fileIndex");
    token_reg.def((GetTokenFunc)&Token::next, "next");
    token_reg.def((GetTokenFunc)&Token::previous, "previous");
    token_reg.def((GetUIntFunc)&Token::varId, "varId");
    token_reg.def((printOut1)&Token::printOut, "printOut");
    token_reg.def((GetTokenFunc)&Token::link, "link");
    token_reg.def((GetScopeFunc)&Token::scope, "scope");
    token_reg.def((GetFunctionFunc)&Token::function, "_function");
    token_reg.def((GetVariableFunc)&Token::variable, "variable");
    token_reg.def(&Token::strValue, "strValue");
    token_reg.def(&Token::progressValue, "progressValue");
    token_reg.def(&Token::nextArgument, "nextArgument");
    token_reg.def((GetConstTokenFunc)&Token::findClosingBracket, "findClosingBracket");
    token_reg.def((GetStringFunc)&Token::originalName, "originalName");
    token_reg.def(&Token::getValue, "getValue");
    token_reg.def(&Token::getMaxValue, "getMaxValue");
    token_reg.def(&Token::getValueLE, "getValueLE");
    token_reg.def(&Token::getValueGE, "getValueGE");
    token_reg.def(&Token::getValueTokenMaxStrLength, "getValueTokenMaxStrLength");
    token_reg.def(&Token::getValueTokenMinStrSize, "getValueTokenMinStrSize");
    token_reg.def(&Token::getValueTokenDeadPointer, "getValueTokenDeadPointer");

    fflua_register_t<> static_reg(L);
    typedef Token *(*findsimplematch2)(Token *tok, const char pattern[]);
    typedef Token *(*findsimplematch3)(Token *tok, const char pattern[], const Token *end);
    typedef Token *(*findmatch3)(Token *tok, const char pattern[], unsigned int varId);
    typedef Token *(*findmatch4)(Token *tok, const char pattern[], const Token *end, unsigned int varId);
    //static_reg.def((findsimplematch2)&Token::findsimplematch, "findsimplematch");
    static_reg.def((findsimplematch3)&Token::findsimplematch, "findsimplematch");
    //static_reg.def((findmatch3)&Token::findmatch, "findmatch");
    static_reg.def((findmatch4)&Token::findmatch, "findmatch");
    static_reg.def(&Token::multiCompare, "multiCompare");

    static_reg.def(&IsSameToken, "IsSameToken");
    static_reg.def(&Token::simpleMatch, "simpleMatch");
    static_reg.def(&Token::Match, "Match");
    static_reg.def(&Token::getStrLength, "getStrLength");
    static_reg.def(&Token::getStrSize, "getStrSize");
    static_reg.def(&Token::getCharAt, "getCharAt");

    std::map<std::string, int> tokenType;
    tokenType["eVariable"] = Token::eVariable;
    tokenType["eType"] = Token::eType;
    tokenType["eFunction"] = Token::eFunction;
    tokenType["eKeyword"] = Token::eKeyword;
    tokenType["eName"] = Token::eName;
    tokenType["eNumber"] = Token::eNumber;
    tokenType["eString"] = Token::eString;
    tokenType["eChar"] = Token::eChar;
    tokenType["eBoolean"] = Token::eBoolean;
    tokenType["eLiteral"] = Token::eLiteral;
    tokenType["eArithmeticalOp"] = Token::eArithmeticalOp;
    tokenType["eComparisonOp"] = Token::eComparisonOp;
    tokenType["eAssignmentOp"] = Token::eAssignmentOp;
    tokenType["eLogicalOp"] = Token::eLogicalOp;
    tokenType["eBitOp"] = Token::eBitOp;
    tokenType["eIncDecOp"] = Token::eIncDecOp;
    tokenType["eExtendedOp"] = Token::eExtendedOp;
    tokenType["eBracket"] = Token::eBracket;
    tokenType["eOther"] = Token::eOther;
    tokenType["eNone"] = Token::eNone;

    ff::set_global_variable(L, "TokenType", tokenType);
}

void defTokenizer(lua_State* L)
{
    fflua_register_t<Tokenizer, int()> tokenizer_reg(L, "Tokenizer");
    tokenizer_reg.def(&Tokenizer::isC, "isC");
    tokenizer_reg.def(&Tokenizer::isCPP, "isCPP");
    tokenizer_reg.def(&Tokenizer::getSymbolDatabase, "getSymbolDatabase");
    tokenizer_reg.def(&Tokenizer::tokens, "tokens");
}

void defSeverity(lua_State* L)
{
    std::map<std::string, int> severityType;
    severityType["none"] = Severity::none;
    severityType["error"] = Severity::error;
    severityType["warning"] = Severity::warning;
    severityType["style"] = Severity::style;
    severityType["performance"] = Severity::performance;
    severityType["portability"] = Severity::portability;
    severityType["information"] = Severity::information;
    severityType["debug"] = Severity::debug;

    ff::set_global_variable(L, "SeverityType", severityType);
}

void defScope(lua_State* L)
{
    typedef const Scope *(Scope::*GetScopeFunc)(const std::string&) const;
    typedef const Type *(Scope::*GetTypeFunc)(const std::string&) const;

    fflua_register_t<Scope, int()> scope_reg(L, "Scope");
    scope_reg.def(const_cast<SymbolDatabase *Scope::*>(&Scope::check), "check");
    scope_reg.def(&Scope::className, "className");
    scope_reg.def(const_cast<Token *Scope::*>(&Scope::classDef), "classDef");
    scope_reg.def(const_cast<Token *Scope::*>(&Scope::classStart), "classStart");
    scope_reg.def(const_cast<Token *Scope::*>(&Scope::classEnd), "classEnd");
//fixme    scope_reg.def(&Scope::functionList, "functionList");
//fixme scope_reg.def(&Scope::varlist, "varlist");
    scope_reg.def(const_cast<Scope *Scope::*>(&Scope::nestedIn), "nestedIn");
    scope_reg.def(&Scope::nestedList, "nestedList");
    scope_reg.def(&Scope::numConstructors, "numConstructors");
    scope_reg.def(&Scope::numCopyOrMoveConstructors, "numCopyOrMoveConstructors");
//fixme    scope_reg.def(&Scope::usingList, "usingList");
    scope_reg.def(&Scope::type, "type");
    scope_reg.def(&Scope::definedType, "definedType");
    scope_reg.def(&Scope::definedTypes, "definedTypes");
    scope_reg.def(const_cast<Scope *Scope::*>(&Scope::functionOf), "functionOf");
    scope_reg.def(&Scope::function, "_function");
    
    scope_reg.def(&Scope::isClassOrStruct, "isClassOrStruct");
    scope_reg.def(&Scope::isExecutable, "isExecutable");
    scope_reg.def(&Scope::isLocal, "isLocal");
    scope_reg.def(&Scope::findFunction, "findFunction");
    scope_reg.def(&Scope::findInNestedList, "findInNestedList");
    scope_reg.def((GetScopeFunc)&Scope::findRecordInNestedList, "findRecordInNestedList");
    scope_reg.def((GetTypeFunc)&Scope::findType, "findType");
    scope_reg.def(&Scope::findInNestedListRecursive, "findInNestedListRecursive");
    scope_reg.def(&Scope::getVariableList, "getVariableList");
    scope_reg.def(&Scope::getDestructor, "getDestructor");
    scope_reg.def(&Scope::getNestedNonFunctions, "getNestedNonFunctions");
    scope_reg.def(&Scope::hasDefaultConstructor, "hasDefaultConstructor");
    scope_reg.def(&Scope::defaultAccess, "defaultAccess");
//fixme    scope_reg.def(&Scope::checkVariable, "checkVariable");
    scope_reg.def(&Scope::getVariable, "getVariable");;

    std::map<std::string, int> scopeType;
    scopeType["eGlobal"] = Scope::eGlobal;
    scopeType["eClass"] = Scope::eClass;
    scopeType["eStruct"] = Scope::eStruct;
    scopeType["eUnion"] = Scope::eUnion;
    scopeType["eNamespace"] = Scope::eNamespace;
    scopeType["eFunction"] = Scope::eFunction;
    scopeType["eIf"] = Scope::eIf;
    scopeType["eElse"] = Scope::eElse;
    scopeType["eFor"] = Scope::eFor;
    scopeType["eWhile"] = Scope::eWhile;
    scopeType["eDo"] = Scope::eDo;
    scopeType["eSwitch"] = Scope::eSwitch;
    scopeType["eUnconditional"] = Scope::eUnconditional;
    scopeType["eTry"] = Scope::eTry;
    scopeType["eCatch"] = Scope::eCatch;
    scopeType["eLambda"] = Scope::eLambda;

    ff::set_global_variable(L, "ScopeType", scopeType);
}

void defType(lua_State* L)
{
    fflua_register_t<Type, int()> type_reg(L, "Type");
    type_reg.def(const_cast<Token *Type::*>(&Type::classDef), "classDef");
    type_reg.def(const_cast<Scope *Type::*>(&Type::classScope), "classScope");
    type_reg.def(const_cast<Scope *Type::*>(&Type::enclosingScope), "enclosingScope");
//fixme    type_reg.def(&Type::derivedFrom, "derivedFrom");
//fixme    type_reg.def(&Type::friendList, "friendList");

    type_reg.def(&Type::name, "name");
    type_reg.def(&Type::initBaseInfo, "initBaseInfo");
    type_reg.def(&Type::getFunction, "getFunction");
    type_reg.def(&Type::hasCircularDependencies, "hasCircularDependencies");
}

void defFunction(lua_State* L)
{
    fflua_register_t<Function, int()> function_reg(L, "Function");

    function_reg.def(const_cast<Token *Function::*>(&Function::tokenDef), "tokenDef");
    function_reg.def(const_cast<Token *Function::*>(&Function::argDef), "argDef");
    function_reg.def(const_cast<Token *Function::*>(&Function::token), "token");
    function_reg.def(const_cast<Token *Function::*>(&Function::arg), "arg");
    function_reg.def(const_cast<Token *Function::*>(&Function::retDef), "retDef");
    function_reg.def(const_cast<Type *Function::*>(&Function::retType), "retType");
    function_reg.def(const_cast<Scope *Function::*>(&Function::functionScope), "functionScope");
    function_reg.def(const_cast<Scope *Function::*>(&Function::nestedIn), "nestedIn");
//fixme    function_reg.def(&Function::argumentList, "argumentList");
    function_reg.def(&Function::initArgCount, "initArgCount");
    function_reg.def(&Function::type, "type");
    function_reg.def(&Function::access, "access");
    function_reg.def(&Function::hasBody, "hasBody");
    function_reg.def(&Function::isInline, "isInline");
    function_reg.def(&Function::isConst, "isConst");
    function_reg.def(&Function::isVirtual, "isVirtual");
    function_reg.def(&Function::isPure, "isPure");
    function_reg.def(&Function::isStatic, "isStatic");
    function_reg.def(&Function::isFriend, "isFriend");
    function_reg.def(&Function::isExplicit, "isExplicit");
    function_reg.def(&Function::isDefault, "isDefault");
    function_reg.def(&Function::isDelete, "isDelete");
    function_reg.def(&Function::isNoExcept, "isNoExcept");
    function_reg.def(&Function::isThrow, "isThrow");
    function_reg.def(&Function::isOperator, "isOperator");
    function_reg.def(const_cast<Token *Function::*>(&Function::noexceptArg), "noexceptArg");
    function_reg.def(const_cast<Token *Function::*>(&Function::throwArg), "throwArg");

    function_reg.def(&Function::name, "name");
    function_reg.def(&Function::argCount, "argCount");
    function_reg.def(&Function::minArgCount, "minArgCount");
    function_reg.def(&Function::getArgumentVar, "getArgumentVar");
    function_reg.def(&Function::initializedArgCount, "initializedArgCount");
    function_reg.def(&Function::addArguments, "addArguments");
    function_reg.def(&Function::isImplicitlyVirtual, "isImplicitlyVirtual");
    function_reg.def(&Function::isConstructor, "isConstructor");
    function_reg.def(&Function::isDestructor, "isDestructor");
    function_reg.def(&Function::isAttributeConstructor, "isAttributeConstructor");
    function_reg.def(&Function::isAttributeDestructor, "isAttributeDestructor");
    function_reg.def(&Function::isAttributePure, "isAttributePure");
    function_reg.def(&Function::isAttributeConst, "isAttributeConst");
    function_reg.def(&Function::isAttributeNothrow, "isAttributeNothrow");
    function_reg.def(&Function::isDeclspecNothrow, "isDeclspecNothrow");

    std::map<std::string, int> functionType;
    functionType["eConstructor"] = Function::eConstructor;
    functionType["eCopyConstructor"] = Function::eCopyConstructor;
    functionType["eMoveConstructor"] = Function::eMoveConstructor;
    functionType["eOperatorEqual"] = Function::eOperatorEqual;
    functionType["eDestructor"] = Function::eDestructor;
    functionType["eFunction"] = Function::eFunction;

    ff::set_global_variable(L, "FunctionType", functionType);
}

void defVariable(lua_State* L)
{
    typedef const Type *(Variable::*GetTypeFunc)() const;
    typedef bool(Variable::*GetBoolFunc)() const;

    fflua_register_t<Variable, int()> variable_reg(L, "Variable");
    variable_reg.def(&Variable::nameToken, "nameToken");
    variable_reg.def(&Variable::typeStartToken, "typeStartToken");
    variable_reg.def(&Variable::typeEndToken, "typeEndToken");
    variable_reg.def(&Variable::name, "name");
    variable_reg.def(&Variable::declarationId, "declarationId");
    variable_reg.def(&Variable::index, "index");
    variable_reg.def(&Variable::isPublic, "isPublic");
    variable_reg.def(&Variable::isProtected, "isProtected");
    variable_reg.def(&Variable::isPrivate, "isPrivate");
    variable_reg.def(&Variable::isGlobal, "isGlobal");
    variable_reg.def(&Variable::isNamespace, "isNamespace");
    variable_reg.def(&Variable::isArgument, "isArgument");
    variable_reg.def(&Variable::isLocal, "isLocal");
    variable_reg.def(&Variable::isMutable, "isMutable");
    variable_reg.def(&Variable::isStatic, "isStatic");
    variable_reg.def(&Variable::isExtern, "isExtern");
    variable_reg.def(&Variable::isConst, "isConst");
    variable_reg.def(&Variable::isThrow, "isThrow");
    variable_reg.def(&Variable::isClass, "isClass");
    variable_reg.def(&Variable::isArray, "isArray");
    variable_reg.def(&Variable::isPointer, "isPointer");
    variable_reg.def(&Variable::isArrayOrPointer, "isArrayOrPointer");
    variable_reg.def(&Variable::isReference, "isReference");
    variable_reg.def(&Variable::isRValueReference, "isRValueReference");
    variable_reg.def(&Variable::hasDefault, "hasDefault");
    variable_reg.def((GetTypeFunc)&Variable::type, "type");
    variable_reg.def(&Variable::typeScope, "typeScope");
    variable_reg.def(&Variable::scope, "scope");
//fixme    variable_reg.def(&Variable::dimensions, "dimensions");
    variable_reg.def(&Variable::dimension, "dimension");
    variable_reg.def(&Variable::dimensionKnown, "dimensionKnown");
    variable_reg.def((GetBoolFunc)&Variable::isStlType, "isStlType");
    variable_reg.def(&Variable::isStlStringType, "isStlStringType");
    variable_reg.def(&Variable::isFloatingType, "isFloatingType");
    variable_reg.def(&Variable::isIntegralType, "isIntegralType");
}

void defSymbolDatabase(lua_State* L)
{
    typedef const Type *(SymbolDatabase::*GetTypeFunc)(const Token *, const Scope *) const;
    typedef const Scope *(SymbolDatabase::*GetScopeFunc)(const Token *, const Scope *) const;

    fflua_register_t<SymbolDatabase, int()> database_reg(L, "SymbolDatabase");
//fixme    database_reg.def(&SymbolDatabase::scopeList, "scopeList");
    database_reg.def(&SymbolDatabase::functionScopes, "functionScopes");
    database_reg.def(&SymbolDatabase::classAndStructScopes, "classAndStructScopes");
//fixme    database_reg.def(&SymbolDatabase::typeList, "typeList");

    database_reg.def(&SymbolDatabase::findVariableType, "findVariableType");
    database_reg.def(&SymbolDatabase::findFunction, "findFunction");
    database_reg.def(&SymbolDatabase::findScopeByName, "findScopeByName");
    database_reg.def((GetTypeFunc)&SymbolDatabase::findType, "findType");
    database_reg.def((GetScopeFunc)&SymbolDatabase::findScope, "findScope");
    database_reg.def(&SymbolDatabase::isClassOrStruct, "isClassOrStruct");
    database_reg.def(&SymbolDatabase::getVariableFromVarId, "getVariableFromVarId");
    database_reg.def(&SymbolDatabase::getVariableListSize, "getVariableListSize");
    database_reg.def(&SymbolDatabase::debugMessage, "debugMessage");
    database_reg.def(&SymbolDatabase::printOut, "printOut");
    database_reg.def(&SymbolDatabase::printVariable, "printVariable");
//fixme database_reg.def(&SymbolDatabase::printXml, "printXml");
}

void defSettings(lua_State* L)
{
    fflua_register_t<Settings, int()> settings_reg(L, "Settings");

    settings_reg.def(&Settings::debug, "_debug");
    settings_reg.def(&Settings::debugwarnings, "debugwarnings");
    settings_reg.def(&Settings::debugFalsePositive, "debugFalsePositive");
    settings_reg.def(&Settings::dump, "dump");
    settings_reg.def(&Settings::exceptionHandling, "exceptionHandling");
    settings_reg.def(&Settings::inconclusive, "inconclusive");
    settings_reg.def(&Settings::experimental, "experimental");
    settings_reg.def(&Settings::_errorsOnly, "_errorsOnly");
    settings_reg.def(&Settings::_inlineSuppressions, "_inlineSuppressions");
    settings_reg.def(&Settings::_verbose, "_verbose");
    settings_reg.def(&Settings::terminate, "terminate");
    settings_reg.def(&Settings::terminated, "terminated");
    settings_reg.def(&Settings::_force, "_force");
    settings_reg.def(&Settings::_relativePaths, "_relativePaths");
    settings_reg.def(&Settings::_basePaths, "_basePaths");
    settings_reg.def(&Settings::_xml, "_xml");
    settings_reg.def(&Settings::_xml_version, "_xml_version");
    settings_reg.def(&Settings::_jobs, "_jobs");
    settings_reg.def(&Settings::_loadAverage, "_loadAverage");
    settings_reg.def(&Settings::_exitCode, "_exitCode");
    settings_reg.def(&Settings::_outputFormat, "_outputFormat");
    settings_reg.def(&Settings::_showtime, "_showtime");
    settings_reg.def(&Settings::_includePaths, "_includePaths");
    typedef const std::string &(Settings::*GetAppendFunc_t)() const;
    settings_reg.def((GetAppendFunc_t)&Settings::append, "append");
    settings_reg.def(&Settings::_maxConfigs, "_maxConfigs");
    settings_reg.def(&Settings::isEnabled, "isEnabled");
    settings_reg.def(&Settings::isEnabledCheck, "isEnabledCheck");
    settings_reg.def(&Settings::addEnabled, "addEnabled");
    settings_reg.def(&Settings::addEnabledCheck, "addEnabledCheck");
    settings_reg.def(&Settings::clearEnabled, "clearEnabled");
    settings_reg.def(&Settings::enforcedLang, "enforcedLang");
//fixme    settings_reg.def(&Settings::nomsg, "nomsg");
//fixme    settings_reg.def(&Settings::nofail, "nofail");
    settings_reg.def(&Settings::userDefines, "userDefines");
    settings_reg.def(&Settings::userUndefs, "userUndefs");
    settings_reg.def(&Settings::userIncludes, "userIncludes");
    settings_reg.def(&Settings::configExcludePaths, "configExcludePaths");
    settings_reg.def(&Settings::reportProgress, "reportProgress");
//fixme    settings_reg.def(&Settings::library, "library");
    settings_reg.def(&Settings::checkConfiguration, "checkConfiguration");
    settings_reg.def(&Settings::checkLibrary, "checkLibrary");
//fixme    settings_reg.def(&Settings::standards, "standards");
    settings_reg.def(&Settings::sizeof_bool, "sizeof_bool");
    settings_reg.def(&Settings::sizeof_short, "sizeof_short");
    settings_reg.def(&Settings::sizeof_int, "sizeof_int");
    settings_reg.def(&Settings::sizeof_long, "sizeof_long");
    settings_reg.def(&Settings::sizeof_long_long, "sizeof_long_long");
    settings_reg.def(&Settings::sizeof_float, "sizeof_float");
    settings_reg.def(&Settings::sizeof_double, "sizeof_double");
    settings_reg.def(&Settings::sizeof_long_double, "sizeof_long_double");
    settings_reg.def(&Settings::sizeof_wchar_t, "sizeof_wchar_t");
    settings_reg.def(&Settings::sizeof_size_t, "sizeof_size_t");
    settings_reg.def(&Settings::sizeof_pointer, "sizeof_pointer");
//fixme    settings_reg.def(&Settings::platform, "platform");
    settings_reg.def(&Settings::platformFile, "platformFile");
    settings_reg.def(&Settings::isWindowsPlatform, "isWindowsPlatform");
    settings_reg.def(&Settings::configurationExcluded, "configurationExcluded");
}

void defLuaPlugin(lua_State* L)
{
    fflua_register_t<LuaPlugin, int()> luaplugin_reg(L, "LuaPlugin");

    luaplugin_reg.def(&LuaPlugin::luaReportError, "reportError");
}

void LuaPlugin::regLuaClasses(ff::fflua_t& fflua)
{
	fflua.reg(defToken);
    fflua.reg(defTokenizer);
    fflua.reg(defSeverity);
    fflua.reg(defScope);
    fflua.reg(defType);
    fflua.reg(defFunction);
    fflua.reg(defVariable);
    fflua.reg(defSymbolDatabase);
    fflua.reg(defLuaPlugin);
    fflua.reg(defSettings);
}

void LuaPlugin::runSimplifiedChecks()
{
    //lua_tinker::class_add<AccessControl>(L, "AccessControl");
    //lua_tinker::class_add<ValueFlow::Value>(L, "ValueFlow.Value");

    fflua_t fflua;
	std::string exe_dir = Path::getPathFromFilename(Path::getModuleFileName());
	std::string lua_plugin_dir = exe_dir + "/checkers";
	fflua.add_package_path(lua_plugin_dir);

	regLuaClasses(fflua);

    fflua.set_global_variable("_checkPlugin", this);
    fflua.set_global_variable("_tokenizer", this->_tokenizer);
    fflua.set_global_variable("_settings", this->_settings);

    try {
        fflua.load_file(this->luaFile_);

	    if (fflua.is_function_exists("runSimplifiedChecks"))
	    {
		    fflua.call("runSimplifiedChecks");
	    }
    }
    catch (lua_exception_t& e)
    {
        std::cerr << e.what() << std::endl;
    }
}
//---------------------------------------------------------------------------

void LuaPlugin::runChecks()
{
	fflua_t fflua;
	std::string exe_dir = Path::getPathFromFilename(Path::getModuleFileName());
	std::string lua_plugin_dir = exe_dir + "/checkers";
	fflua.add_package_path(lua_plugin_dir);

	regLuaClasses(fflua);

	fflua.set_global_variable("_checkPlugin", this);
	fflua.set_global_variable("_tokenizer", this->_tokenizer);
	fflua.set_global_variable("_settings", this->_settings);

    try
    {
	    fflua.load_file(this->luaFile_);

        if (fflua.is_function_exists("runChecks"))
        {
            fflua.call("runChecks");
        }
    }
    catch (lua_exception_t& e)
    {
        std::cerr << e.what() << std::endl;
    }

}

void LuaPlugin::luaReportError(const Token *tok, const Severity::SeverityType severity, const char* id, const char* msg, bool inconclusive) {
    reportError(tok, severity, id, msg, inconclusive);
}
