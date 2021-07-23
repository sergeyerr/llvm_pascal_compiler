#pragma once
#include "EOperationType.h"

enum ECloseBracket {
	__begin__close__ = EOperationType::CloseBracket,
	CloseCurlyBracket,
	CloseSquareBracket,
	CloseRoundBracket,
	__end__close__
};