#include "Automata.h"


bool Automata::MoveToNextChar() {
	if (currentPos < program.length()) {
		currentPos++;
		return true;
	}
	return false;
}

bool Automata::checkInKeyWords(std::string s) {
	return Automata::keyWords.find(s) != keyWords.end();
}

bool Automata::checkIsLetter(char c) {
	return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

bool Automata::checkIsNumeral(char c) {
	return (c >= '0' && c <= '9');
}

bool Automata::checkIsSpace(char c) {
	return c == ' ' || c == '\t' || c == '\n';
}

bool Automata::checkIsSymbol(char c) {
	return checkIsNumeral(c) || checkIsLetter(c) || checkIsSpace(c) ||  checkInKeyWords(std::string(1, c));
}

AutomataMessage Automata::GetCurrentToken() {
	state = AutomataStates::StartState;
	foreheadPos = currentPos;
	while (foreheadPos < program.length() && (Automata::transitions.find(state) != Automata::transitions.end())) {
		char analyzedSymbol = program[foreheadPos];
// только в случае строковых констант принимаем неверные символы
// не придумал, как сделать адекватную обработку без этих костылей в if
		if ((state != AutomataStates::StringCharConstState) && (!checkIsSymbol(analyzedSymbol)
			&& (analyzedSymbol != '\'')
			&& (analyzedSymbol != '_'))) 
		{
			state = AutomataStates::ErrorWrongSymbol;
		}
		else 
			Automata::transitions[state](*this, analyzedSymbol);
		foreheadPos++;
	}
	AutomataMessage res;
// тут есть скрытая модификация current pos в automata, который этот иф и обрабатывает
// перед концом файла всегда есть \n
	if (currentPos == program.length()) {
		res.EndOfFile = true;
		res.Error = false;
		res.TokenType = ETokenType::Corrupted;
		return res;
	}
	res.Sequence = program.substr(currentPos, foreheadPos - currentPos);
	res.TokenStart = currentPos;
	res.TokenEnd = foreheadPos - 1;
	switch (state)
	{
	case AutomataStates::CreateCharOrStringState:
		res.Error = false;
		res.TokenType = ETokenType::Value;
		if (res.Sequence.length() < 3) throw "it is not a char/string token...";
		if (res.Sequence.length() == 3) res.ValueType = EValueType::CharValue;
		else res.ValueType = EValueType::StringValue;
		// убираем ковычки из строковой константы
		res.Sequence = res.Sequence.substr(1, res.Sequence.length() - 2);
		break;
	case AutomataStates::CreateIntState:
		res.Error = false;
		res.TokenType = ETokenType::Value;
		res.ValueType = EValueType::IntegerValue;
		break;
	case AutomataStates::CreateFloatState:
		res.Error = false;
		res.TokenType = ETokenType::Value;
		res.ValueType = EValueType::FloatValue;
		break;
	case AutomataStates::CreateOperationState:
		res.Error = false;
		res.TokenType = ETokenType::Operation;
		break;
	case AutomataStates::CreateIdentiferToken:
		res.Error = false;
		if (checkInKeyWords(res.Sequence)) res.TokenType = ETokenType::Operation;
		else res.TokenType = ETokenType::Identifier;
		break;
	case AutomataStates::ErrorWrongFloatConstant:
		res.ErrorType = ELexicalErrorType::WrongFloatConstant;
		res.TokenStart = currentPos;
		res.TokenEnd = foreheadPos - 1;
		break;
	case AutomataStates::ErrorStringExceedsLine:
		res.ErrorType = ELexicalErrorType::StringExceedsLine;
		res.TokenStart = currentPos;
		res.TokenEnd = foreheadPos - 1;
		break;
	case AutomataStates::ErrorWrongSymbol:
		res.ErrorType = ELexicalErrorType::WrongSymbol;
		res.TokenStart = foreheadPos - 2;
		res.TokenEnd = foreheadPos - 1;
		break;
	default:
		throw "No such state in Automata, check switch and transitions map";
		break;
	}
	return res;

}

AutomataMessage Automata::PopCurrentToken() {
	auto tmp = Automata::GetCurrentToken();
	currentPos = foreheadPos;
	return tmp;
}



