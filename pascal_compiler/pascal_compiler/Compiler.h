#pragma once
#include "LexicalAnalyzer.h"
#include "SemanticTable.h"
#include "ASTReturn.h"
#include "codeGenerator.h"

class SyntaxError : public IError {
public:
	const std::string text;
	SyntaxError(std::string str, int sL, int eL, int  sC, int eC) : IError(sL, eL, sC, eC, EErrorType::Syntax), text(str) {};
};


class FailedToRecoverError : public IError {
public:
	FailedToRecoverError() : IError(-1, -1, -1, -1, EErrorType::FailedToRecover) {};
};

class Compiler {
	LexicalAnalyzer lexer;
	SemanticTable sem_table;
	CodeGenerator LLVMCodeGenerator;
	std::unique_ptr<IToken> currentToken;
	std::pair<int, int> startPos, endPos;
	void accept(OperationEnumWrapper oper, bool expect_end = false);
	void next_token(bool expect_end = false);
	void saveError(IError*);
	void throwSyntaxError(std::string);
	void init_semantics();
	void checkOperation();
	bool analyzed = false;
	bool is_compilable = true;
	bool eof = false;
	std::string output_file;

public:
	Compiler(std::string input, std::string output);
	~Compiler();
	void Start();
	std::vector<std::string> errorBus;
	bool IsParsed();
	bool IsCompilable();
	int Compile();
private:
	ASTReturn program();

	ASTReturn block();

	ASTReturn variableSection();

	ASTReturn compositeOperator();


	ASTReturn whileOperator();
	ASTReturn ifOperator();
	ASTReturn unmarkedOperator();
	ASTReturn assignOper();
	ASTReturn repeatOperator();
	ASTReturn forOperator();


	ASTReturn variable();
	ASTReturn constantNoSign();
	ASTReturn constant();


	ASTReturn simpleExpression();
	ASTReturn expression();
	ASTReturn term();
	ASTReturn mult();

	ASTReturn typesSection();
	ASTReturn typeDefinition();
	ASTReturn type();
	ASTReturn simpleType();
	ASTReturn enumerableType();
	ASTReturn intervalType();
	ASTReturn arrayType();

};