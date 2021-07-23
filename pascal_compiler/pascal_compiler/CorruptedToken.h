#pragma once
#include <string>
#include "IToken.h"
#include "ETokenType.h"

class CorruptedToken : public IToken {
public:
	CorruptedToken() : IToken(ETokenType::Corrupted) { };
};