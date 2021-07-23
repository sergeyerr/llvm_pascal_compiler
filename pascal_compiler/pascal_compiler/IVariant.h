#pragma once
#include "EValueType.h"

class IVariant {
public:
	const EValueType Type;
protected:
	IVariant(EValueType type) : Type(type) {};
};