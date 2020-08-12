#include "parser_comp.hpp"

void Parser::dependencies(ASTNode *& root)
{
    ASTNode * globalNode = ASTUtility::createASTNode(DEPENDENCY_GLOBAL, NULL, NULL);
    ASTNode * functionNode = ASTUtility::createASTNode(DEPENDENCY_FUNC, globalNode, NULL);
    root->right = functionNode;
    while (currToken->tType == T_KEYWORD)
    {
        if (currToken->entry == "function")
        {
            if ((currToken + 1)->entry == "MAIN")
            {
                return;
            }
            else 
            {
                functionNode->right = ASTUtility::createASTNode(DEPENDENCY, this->function(), NULL);
                functionNode = functionNode->right;
            }
        }
        else if ((currToken->entry == "let") || 
            (currToken->entry == "const") )
        {
            globalNode->right = ASTUtility::createASTNode(DEPENDENCY, this->globalAssignment(), NULL);
            globalNode = globalNode->right;
        }
        else if (currToken->entry == "import")
        {
            ASTUtility::validToken(T_KEYWORD, currToken->entry, currToken);

            ASTNode * importLib = this->importFile(root);
            
            ASTUtility::validToken(T_SYMBOL, ";", currToken);
            
            globalNode->right = ASTUtility::createASTNode(IMPORT, importLib, NULL);
        }
    }
}

void Parser::initGrammar(TokenEntry * token_instance, ASTNode *& the_ast) 
{
    currToken = token_instance;
    start(the_ast);
}

ASTNode * Parser::main()
{
    ASTNode* mainNode = ASTUtility::createASTNode(MAIN, NULL, NULL);
    map<string, bool> placeholder_symbol_table = symbol_table;

    ASTUtility::validToken(T_KEYWORD, "function", currToken);
    ASTUtility::validToken(T_VARIABLE, "MAIN", currToken);
    ASTUtility::validToken(T_SYMBOL, "(", currToken);
    ASTUtility::validToken(T_SYMBOL, ")", currToken);
    ASTUtility::validToken(T_SYMBOL, "{", currToken);
    this->stmtList(mainNode);
    symbol_table.clear();
    ASTUtility::validToken(T_SYMBOL, "}", currToken);
    return mainNode;
}

void Parser::start(ASTNode *& root)
{
    try
    {
        root = ASTUtility::createASTNode(UNKNOWN, ASTUtility::createASTNullNode(), ASTUtility::createASTNullNode());
        dependencies(root);
        root = ASTUtility::createASTNode(START, root, main());
    }
    catch(int e)
    {
        ErrorReader::readError(e, currToken->entry);
        root =  NULL;
    }
}