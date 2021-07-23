#include "LexicalAnalyzer.h"
#include "Automata.h"
#include "IdentifierToken.h"
#include "OperationToken.h"
#include "ValueToken.h"
#include <set>

LexicalAnalyzer::LexicalAnalyzer(std::string filename) {
	auto lines = ReadLinesFromFile(filename);
	for (auto p : keywordDict) {
		reverseKeywordDict[p.second] = p.first;
	}
	DeleteLineComments(lines);
	auto prog = MergeLines(lines);
	
	int col, line;
	col = line = 0;
	for (int i = 0; i < prog.length(); i++) {
		posToLineCol.push_back({ line, col });
		if (prog[i] == '\n') {
			line += 1;
			col = 0;
		}
		else col += 1;
	}

	DeleteBlockComments(prog);
	std::set<std::string> keywords;
	for (auto pair : LexicalAnalyzer::keywordDict) {
		keywords.insert(pair.first);
	}
	automata = Automata(prog, keywords);
}

std::unique_ptr<IToken> LexicalAnalyzer::createToken(AutomataMessage automata_msg) {
	std::unique_ptr<IToken> token;
	switch (automata_msg.TokenType)
	{
	case ETokenType::Corrupted:
		token = nullptr;
		break;
	case ETokenType::Identifier:
		token = std::make_unique<IdentifierToken>(automata_msg.Sequence);
		break;
	case ETokenType::Value:
		switch (automata_msg.ValueType)
		{
		case EValueType::CharValue:
			if (automata_msg.Sequence.length() > 1) throw "Char with more than 1 char???";
			token = std::make_unique<ValueToken>(automata_msg.Sequence[0]);
			break;
		case EValueType::StringValue:
			token = std::make_unique<ValueToken>(automata_msg.Sequence);
			break;
		case EValueType::IntegerValue:
			token = std::make_unique<ValueToken>(std::stoi(automata_msg.Sequence));
			break;
		case EValueType::FloatValue:
			token = std::make_unique<ValueToken>(std::stof(automata_msg.Sequence));
			break;
		}
		break;
	case ETokenType::Operation:
		token = std::make_unique<OperationToken>(keywordDict[automata_msg.Sequence]);
		break;
	}
	return token;
}

LexerMessage LexicalAnalyzer::processAutomataMsg(AutomataMessage automata_msg) {

	if (automata_msg.Error) {
		auto start_pos = posToLineCol[automata_msg.TokenStart];
		auto end_pos = posToLineCol[automata_msg.TokenEnd];
		auto error =  new LexicalError(automata_msg.ErrorType, start_pos.first, end_pos.first, start_pos.second, end_pos.second);
		LexerMessage res(automata_msg.EndOfFile, error);
		return res;
	}
	else {
		std::pair<int, int> start_pos, end_pos;
		if (automata_msg.TokenStart >= posToLineCol.size()) {
			start_pos = posToLineCol[posToLineCol.size() - 1];
		}
		else {
			start_pos = posToLineCol[automata_msg.TokenStart];
			
		}
		if (automata_msg.TokenEnd >= posToLineCol.size()) {
			end_pos = posToLineCol[posToLineCol.size() - 1];
		}
		else end_pos = posToLineCol[automata_msg.TokenEnd];
		auto token = std::move(createToken(automata_msg));
		LexerMessage res(automata_msg.EndOfFile, token, start_pos.first, start_pos.second, end_pos.first, end_pos.second);
		return res;
	}
}

LexerMessage LexicalAnalyzer::Read() {
	auto automata_msg = automata.PopCurrentToken();
	auto res = processAutomataMsg(automata_msg);
	return res;
}

LexerMessage LexicalAnalyzer::ReadWhileNot(bool (*condition)(IToken*)) {
	while (true) {
		auto automata_msg = automata.GetCurrentToken();
	    auto res = processAutomataMsg(automata_msg);
		if (res.Token != nullptr) {
			if (condition(res.Token.get())) 
				return res;
		}
		if (!automata.MoveToNextChar()) {
			res.EndOfFile = true;
			return res;
		}
	}
}

