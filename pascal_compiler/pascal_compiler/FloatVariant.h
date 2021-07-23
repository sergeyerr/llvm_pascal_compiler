#pragma once
#include "ETokenType.h"
#include "EValueType.h"
#include "IVariant.h"

class FloatVariant : public IVariant {
public:
	const float FloatValue;
	FloatVariant(float val) : IVariant(EValueType::FloatValue), FloatValue(val) {};
};