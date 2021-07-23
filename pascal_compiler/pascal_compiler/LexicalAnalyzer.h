#pragma once
#include "LexicalError.h"
#include "Automata.h"
#include "OperationEnumWrapper.h"
#include "IToken.h"
#include "StringHelpers.h"
#include <vector>
#include <memory>

struct LexerMessage {
	bool EndOfFile;
	bool IsError = false;
	LexicalError* Error;
	int StartLine = -1, EndLine = -1, StartColumn = -1, EndColumn = -1;
	std::unique_ptr<IToken> Token;
	LexerMessage(bool eof, std::unique_ptr<IToken>& token, int sl, int el, int sc, int ec) : EndOfFile(eof), IsError(false), StartLine(sl), StartColumn(sc), EndLine(el), EndColumn(ec)
	{ Token = std::move(token); };
	LexerMessage(bool eof, LexicalError* e) : EndOfFile(eof), IsError(true), Error(e), Token(nullptr),
		StartColumn(e->StartColumn), StartLine(e->StartLine), EndLine(e->EndLine), EndColumn(e->EndColumn)
	{
	};

};


class LexicalAnalyzer {
	Automata automata;
	std::vector<std::pair<int, int>> posToLineCol;
	std::unique_ptr<IToken> createToken(AutomataMessage);
	LexerMessage processAutomataMsg(AutomataMessage msg);
public:
	static std::map<std::string, OperationEnumWrapper> keywordDict;
	static std::map<OperationEnumWrapper, std::string> reverseKeywordDict;
	LexicalAnalyzer(std::string filename);
	LexerMessage Read();
	LexerMessage ReadWhileNot(bool (*condition)(IToken*));

};
