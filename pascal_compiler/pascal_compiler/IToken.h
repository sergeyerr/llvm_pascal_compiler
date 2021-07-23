#pragma once
#include "ETokenType.h"
#include <iostream>

class IToken {
public:
	const ETokenType Type;
protected:
	IToken(ETokenType type) : Type(type) {};
	friend std::ostream& operator<<(std::ostream& os, const IToken& token);
};