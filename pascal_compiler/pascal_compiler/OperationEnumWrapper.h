#pragma once
#include "EOperationType.h"
#include "EAdditiveOperation.h"
#include "ECloseBracket.h"
#include "EComparisonOperation.h"
#include "EKeyWord.h"
#include "EMultiplicativeOperation.h"
#include "EOpenBraket.h"
#include "ESystemSymbol.h"
#include "ETypeWord.h"


class OperationEnumWrapper {
	int opCode;
public:
	const EOperationType OperationType;

	bool operator==(int other) {
		return opCode == other;
	}

	operator int() const {
		return opCode;
	}

	OperationEnumWrapper() : OperationType(NoOperation), opCode(-1) {};

	OperationEnumWrapper(EAdditiveOperation op) : OperationType(EOperationType::AdditiveOperation), opCode(op) {}

	OperationEnumWrapper(ECloseBracket op) : OperationType(EOperationType::CloseBracket), opCode(op) {}

	OperationEnumWrapper(EComparisonOperation op) : OperationType(EOperationType::ComparisonOperation), opCode(op) {}

	OperationEnumWrapper(EKeyWord op) : OperationType(EOperationType::KeyWord), opCode(op) {}

	OperationEnumWrapper(EMultiplicativeOperation op) : OperationType(EOperationType::MultiplicativeOperation), opCode(op) {}

	OperationEnumWrapper(EOpenBracket op) : OperationType(EOperationType::OpenBracket), opCode(op) {}

	OperationEnumWrapper(ESystemSymbol op) : OperationType(EOperationType::SystemSymbol), opCode(op) {}

	//OperationEnumWrapper(ETypeWord op) : OperationType(EOperationType::TypeWord), opCode(op) {}

	EAdditiveOperation GetAdditiveOperation() {
		if (opCode > EAdditiveOperation::__begin__add__ and opCode < EAdditiveOperation::__end__add__) 
			return EAdditiveOperation(opCode);
		throw "Trying to get operation of wrong type";
	}

	ECloseBracket GetCloseBraket() {
		if (opCode > ECloseBracket::__begin__close__ and opCode < ECloseBracket::__end__close__)
			return ECloseBracket(opCode);
		throw "Trying to get operation of wrong type";
	}

	EComparisonOperation GetComparisonOperation() {
		if (opCode > EComparisonOperation::__begin__comp__ and opCode < EComparisonOperation::__end__comp__)
			return EComparisonOperation(opCode);
		throw "Trying to get operation of wrong type";
	}

	EKeyWord GetKeyWord() {
		if (opCode > EKeyWord::__begin__key__ and opCode < EKeyWord::__end__key__)
			return EKeyWord(opCode);
		throw "Trying to get operation of wrong type";
	}

	EMultiplicativeOperation GetMultiplicativeOperation() {
		if (opCode > EMultiplicativeOperation::__begin__mult__ and opCode < EMultiplicativeOperation::__end__mult__)
			return EMultiplicativeOperation(opCode);
		throw "Trying to get operation of wrong type";
	}

	EOpenBracket GetOpenBraket() {
		if (opCode > EOpenBracket::__begin__open__ and opCode < EOpenBracket::__end__open__)
			return EOpenBracket(opCode);
		throw "Trying to get operation of wrong type";
	}

	ESystemSymbol GetSystemSymbol() {
		if (opCode > ESystemSymbol::__begin__symbol__ and opCode < ESystemSymbol::__end__symbol__)
			return ESystemSymbol(opCode);
		throw "Trying to get operation of wrong type";
	}

	//ETypeWord GetTypeWord() {
	//	if (opCode > ETypeWord::__begin__type__ and opCode < ETypeWord::__end__type__)
	//		return ETypeWord(opCode);
	//	throw "Trying to get operation of wrong type";
	//}
};