#pragma once
#include "ETokenType.h"
#include "EValueType.h"
#include "IVariant.h"

class IntVariant : public IVariant {
public:
	const int IntValue;
	IntVariant(int val) : IVariant(EValueType::IntegerValue), IntValue(val) {};
};