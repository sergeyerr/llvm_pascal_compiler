#pragma once
#include "ETokenType.h"
#include "EValueType.h"
#include "IVariant.h"
#include <string>

class StringVariant : public IVariant {
public:
	const std::string StringValue;
	StringVariant(std::string val) : IVariant(EValueType::StringValue), StringValue(val) {};
};