#pragma once
#include "IError.h"
#include "ELexicalErrorType.h"

class LexicalError : public IError {
public:
	const ELexicalErrorType ErrorType;
	LexicalError(ELexicalErrorType e, int sL, int eL, int  sC, int eC) : IError(sL, eL, sC, eC, EErrorType::Lexical),
		ErrorType(e) {};
	LexicalError() : IError(-1, -1, -1, -1, EErrorType::Lexical), ErrorType(ELexicalErrorType::WrongSymbol) {};
};

