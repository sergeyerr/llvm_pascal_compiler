#pragma once
#include "EOperationType.h"

enum EMultiplicativeOperation {
	__begin__mult__ = EOperationType::MultiplicativeOperation,
	And,
	Multiply,
	Divide,
	Div,
	Mod,
	__end__mult__
};