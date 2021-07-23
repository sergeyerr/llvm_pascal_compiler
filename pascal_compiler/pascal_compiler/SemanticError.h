#pragma once
#include <string>
#include "IError.h"
class SemanticError : public IError {
public:
	const std::string text;
	bool ignorable = false;
	SemanticError(std::string str, int sL, int eL, int  sC, int eC) : IError(sL, eL, sC, eC, EErrorType::Semantic), text(str) {};
	SemanticError(std::string str) : IError(-1, -1, -1, -1, EErrorType::Semantic), text(str) {};
	SemanticError(std::string str, bool ignor) : IError(-1, -1, -1, -1, EErrorType::Semantic), text(str), ignorable(ignor) {};
};