#pragma once
#include "EOperationType.h"

enum EComparisonOperation {
	__begin__comp__ = EOperationType::ComparisonOperation,
	Equal,
	Less,
	LessEqual,
	GreaterEqual,
	Greater,
	In,
	NotEqual,
	__end__comp__
};