#pragma once
#include "IToken.h"
#include "ETokenType.h"
#include "OperationEnumWrapper.h"

class OperationToken : public IToken {
public:
	const OperationEnumWrapper Operation;
	OperationToken(OperationEnumWrapper operation) : IToken(ETokenType::Operation), Operation(operation) { };
};