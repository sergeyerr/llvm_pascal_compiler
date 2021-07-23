#pragma once
enum EOperationType {
	NoOperation = -1,
	CloseBracket = 0x000,
	OpenBracket = 0x100,
	KeyWord = 0x200,
	AdditiveOperation = 0x300,
	MultiplicativeOperation = 0x400,
	ComparisonOperation = 0x500,
	SystemSymbol = 0x600,
	//TypeWord = 0x700
};