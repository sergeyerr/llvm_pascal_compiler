#pragma once
#include <string>
#include "IToken.h"
#include "ETokenType.h"
#include "IType.h"
class IdentifierToken : public IToken {
public:
	const std::string Name;
	IdentifierToken(std::string name) : IToken(ETokenType::Identifier), Name(name) { };
};