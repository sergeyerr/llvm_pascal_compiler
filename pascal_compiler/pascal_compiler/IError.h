#pragma once
#include <exception>

enum class EErrorType {
	Lexical,
	Syntax,
	Semantic,
	FailedToRecover
};

class IError : public std::exception {
public:
	int StartLine, EndLine, StartColumn, EndColumn;
	const EErrorType Type;
protected:
	IError(int sL, int eL, int sC, int eC, EErrorType type) : StartLine(sL),
		EndLine(eL),
		StartColumn(sC),
		EndColumn(eC),
		Type(type)
	{}
};
