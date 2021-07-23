#include "Compiler.h"
#include "OperationToken.h"
#include "CorruptedToken.h"
#include "SemanticError.h"
void Compiler::accept(OperationEnumWrapper op, bool expect_end) {
	checkOperation();
	auto token = static_cast<OperationToken*>(currentToken.get());
	if (token->Operation != op) {
		throwSyntaxError("expected operation " + lexer.reverseKeywordDict[op] +" got another operation " + lexer.reverseKeywordDict[token->Operation]);
	}
	else {
		this->next_token(expect_end);
	}

}

bool Compiler::IsCompilable() {
	return is_compilable;
}

int Compiler::Compile() {
	return LLVMCodeGenerator.GenerateCode("tmp.obj");
}

void Compiler::throwSyntaxError(std::string str) {
	throw new SyntaxError(str,
		this->startPos.first, this->startPos.second, this->endPos.first, this->endPos.second);
}

void Compiler::checkOperation() {
	if (eof) throwSyntaxError("expected operation, but got eof");
	if (currentToken->Type != ETokenType::Operation) {
		throwSyntaxError("expected operation, but got another token type ");
	}
}

void Compiler::next_token(bool expect_end) {
	
	auto msg = lexer.Read();
	this->startPos.first = msg.StartLine;
	this->startPos.second = msg.StartColumn;
	this->endPos.first = msg.EndLine;
	this->endPos.second = msg.EndColumn;
	if (msg.EndOfFile) {
		currentToken = std::make_unique<CorruptedToken>();
		eof = true;
		if (!expect_end)
			throw SyntaxError("unexpected EOF",
				this->startPos.first, this->startPos.second, this->endPos.first, this->endPos.second);

	}
	else if (msg.IsError) {
		throw msg.Error;
	}
	else {
		currentToken = std::move(msg.Token);
		if (currentToken->Type == ETokenType::Corrupted) throwSyntaxError("corruptedToken, idk why it is syntax error, if you see this, fix lexer");
	}
}

void Compiler::saveError(IError* error) {
	analyzed = false;
	is_compilable = false;
	switch (error->Type)
	{
	case (EErrorType::Syntax): {

		SyntaxError* tmp1 = static_cast<SyntaxError*>(error);
		errorBus.push_back("Syntax error| start: " + std::to_string(tmp1->StartLine + 1)
			+ "," + std::to_string(tmp1->StartColumn + 1) + " end: " + std::to_string(tmp1->EndLine + 1) + "," + std::to_string(tmp1->EndColumn + 1)
			+ "| message:" + tmp1->text);
		delete error;
		break;
	}
	case (EErrorType::Lexical):
	{
		LexicalError* tmp2 = static_cast<LexicalError*>(error);
		std::string message = "Lexical error| start: " + std::to_string(tmp2->StartLine + 1)
			+ "," + std::to_string(tmp2->StartColumn + 1) + " end: " + std::to_string(tmp2->EndLine + 1) + "," + std::to_string(tmp2->EndColumn + 1)
			+ "| ";
		switch (tmp2->ErrorType) {
		case ELexicalErrorType::ConstantOutOfRange:
			message += "ConstantOutOfRange";
			break;
		case ELexicalErrorType::StringExceedsLine:
			message += "StringExceedsLine";
			break;
		case ELexicalErrorType::WrongFloatConstant:
			message += "WrongFloatConstant";
			break;
		case ELexicalErrorType::WrongIdentifier:
			message += "WrongIdentifier";
			break;
		case ELexicalErrorType::WrongSymbol:
			message += "WrongSymbol";
			break;
		}
		errorBus.push_back(message);
		delete error;
		break;
	}
	case (EErrorType::Semantic) :
	{
		SemanticError* tmp3 = static_cast<SemanticError*>(error);
		if (tmp3->ignorable) {
			break;
		}
		tmp3->StartLine = startPos.first;
		tmp3->StartColumn = endPos.first;
		tmp3->EndLine = startPos.second;
		tmp3->EndColumn = endPos.second;
		errorBus.push_back("Semantic error| start: " + std::to_string(tmp3->StartLine + 1)
			+ "," + std::to_string(tmp3->StartColumn + 1) + " end: " + std::to_string(tmp3->EndLine + 1) + "," + std::to_string(tmp3->EndColumn + 1)
			+ "| message:" + tmp3->text);
		delete error;
		break;
	}
	default:
		delete error;
		throw "don't use saveError with failed to recoved and other errors";
		break;
	}
}

void Compiler::init_semantics() {
	sem_table.NewFrame();
	sem_table.CreateIdentifier("integer", EIdentifierUsage::Type, sem_table.CreateScalarType("integer"));
	sem_table.CreateIdentifier("boolean", EIdentifierUsage::Type, sem_table.CreateEnumType({  "false", "true" }));
	sem_table.CreateIdentifier("float", EIdentifierUsage::Type, sem_table.CreateScalarType("float"));
	sem_table.CreateIdentifier("char", EIdentifierUsage::Type, sem_table.CreateScalarType("char"));
	//костылюмба - string ограничена длиной 131072
	sem_table.CreateIdentifier("string", EIdentifierUsage::Type, sem_table.CreateArrayType(sem_table.GetBaseType("char"), { sem_table.CreateRangeType(1, 131072) }));
	// костыли для процедур, тут ещё ... параметров для печати
	sem_table.CreateIdentifier("printf", EIdentifierUsage::Procedure, nullptr);
	sem_table.NewFrame();
}

Compiler::Compiler(std::string input, std::string output) : lexer(input), LLVMCodeGenerator(&is_compilable) {
	is_compilable = true;
	output_file = output;
	init_semantics();
	LLVMCodeGenerator.createBoolConsts();
	this->next_token();
}

Compiler::~Compiler() {
	while (sem_table.DeleteFrame()) {};
}

void Compiler::Start() {
	analyzed = true;
	program();
}

bool Compiler::IsParsed() {
	return analyzed;
}