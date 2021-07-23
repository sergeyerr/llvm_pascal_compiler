#pragma once
#include "EOperationType.h"

enum ESystemSymbol {
	__begin__symbol__ = EOperationType::SystemSymbol,
	Dot,
	Comma,
	Colon,
	Range,
	Semicolon,
	Assign,
	__end__symbol__
};