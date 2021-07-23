#pragma once
#include "ETokenType.h"
#include "EValueType.h"
#include "IVariant.h"

class BoolVariant: public IVariant {
public:
	const bool BoolValue;
	BoolVariant(bool value) : IVariant(EValueType::BoolValue), BoolValue(value) {};
};