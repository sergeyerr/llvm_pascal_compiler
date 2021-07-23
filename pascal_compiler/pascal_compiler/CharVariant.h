#pragma once
#include "ETokenType.h"
#include "EValueType.h"
#include "IVariant.h"

class CharVariant : public IVariant {
public:
	const char CharValue;
	CharVariant(char val) : IVariant(EValueType::CharValue),  CharValue(val) {};
};