#pragma once
#include <string>
#include <set>
#include <map>
#include "ELexicalErrorType.h"
#include "ETokenType.h"
#include "EValueType.h"

struct AutomataMessage {
	bool EndOfFile;
	bool Error;
	ELexicalErrorType ErrorType;
	ETokenType TokenType;
	EValueType ValueType;
	std::string Sequence;
	int TokenStart, TokenEnd;
	AutomataMessage() :  EndOfFile(false), Error(true), TokenType(ETokenType::Corrupted) {};
};

enum class AutomataStates {
	StartState,
	IdentState,
	AlmostDotState,
	AlmostLessState,
	AlmostGreaterState,
	AlmostColonState,
	StringCharConstState,
	IntConstState,
	AlmostFloatConstState,
	FloatConstState,
	CreateCharOrStringState,
	CreateIntState,
	CreateFloatState,
	CreateOperationState,
	CreateIdentiferToken,
	ErrorWrongFloatConstant,
	ErrorStringExceedsLine,
	ErrorWrongSymbol

};

class Automata {
	AutomataStates state;
	std::string program;
	int currentPos, foreheadPos;
	std::set<std::string> keyWords;

	//я уже устал порождать классы, тут есть определённая дизайнерская ошибка
	bool checkInKeyWords(std::string);
	bool checkIsSymbol(char);
	static bool checkIsLetter(char);
	static bool checkIsNumeral(char);
	static bool checkIsSpace(char);


	static void StartState(Automata&, char ch);
	static void IdentState(Automata&, char ch);
	static void AlmostDotState(Automata&, char ch);
	static void AlmostLessState(Automata&, char ch);
	static void AlmostGreaterState(Automata&, char ch);
	static void AlmostColonState(Automata&, char ch);
	static void StringCharConstState(Automata&, char ch);
	static void IntConstState(Automata&, char ch);
	static void AlmostFloatConstState(Automata&, char ch);
	static void FloatConstState(Automata&, char ch);

	//знаю, что можно было заменить на массив, но кому нужна конкретно эта оптимизация, которая мешает читаемости
	//static init? 
// с bound методами  не поработать в фун. парадигме, передаём объект класса в стат. ф. для изменения его состояния 
	std::map<AutomataStates, void(*)(Automata&, char)> transitions = {
		{AutomataStates::StartState, Automata::StartState},
		{AutomataStates::IdentState, Automata::IdentState},
		{AutomataStates::AlmostDotState, Automata::AlmostDotState},
		{AutomataStates::AlmostLessState, Automata::AlmostLessState},
		{AutomataStates::AlmostColonState, Automata::AlmostColonState},
		{AutomataStates::IntConstState, Automata::IntConstState},
		{AutomataStates::StringCharConstState, Automata::StringCharConstState},
		{AutomataStates::AlmostFloatConstState, Automata::AlmostFloatConstState},
		{AutomataStates::FloatConstState, Automata::FloatConstState},
		{AutomataStates::AlmostGreaterState, Automata::AlmostGreaterState}
	};


public:
	Automata(std::string prog, std::set<std::string> kWords) : program(prog), keyWords(kWords), state(AutomataStates::StartState) 
	{ currentPos = foreheadPos = 0; }
	Automata() {};
	bool MoveToNextChar();
	AutomataMessage GetCurrentToken();
	AutomataMessage PopCurrentToken();


};
