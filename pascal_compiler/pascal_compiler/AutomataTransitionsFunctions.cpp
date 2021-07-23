#include "Automata.h"

void Automata::StartState(Automata& a, char ch) {
	if (checkIsSpace(ch)) {
		a.state = AutomataStates::StartState;
		a.currentPos += 1;
		return;
	}
	if (checkIsLetter(ch) || ch == '_') {
		a.state = AutomataStates::IdentState;
		return;
	}
	if (ch == '\'') {
		a.state = AutomataStates::StringCharConstState;
		return;
	}
	if (ch == '.') {
		a.state = AutomataStates::AlmostDotState;
		return;
	}
	if (ch == '<') {
		a.state = AutomataStates::AlmostLessState;
		return;
	}
	if (ch == ':') {
		a.state = AutomataStates::AlmostColonState;
		return;
	}
	if (ch == '>') {
		a.state = AutomataStates::AlmostGreaterState;
		return;
	}
	if (checkIsNumeral(ch)) {
		a.state = AutomataStates::IntConstState;
		return;
	}
	a.state = AutomataStates::CreateOperationState;
}

void Automata::IdentState(Automata& a, char ch) {
	if (checkIsLetter(ch) || checkIsNumeral(ch) || ch == '_') {
		a.state = AutomataStates::IdentState;
		return;
	}
	a.state = AutomataStates::CreateIdentiferToken;
	a.foreheadPos -= 1;

}

void Automata::AlmostDotState(Automata& a, char ch) {
	a.state = AutomataStates::CreateOperationState;
	if (ch != '.') 
		a.foreheadPos -= 1;
}

void Automata::AlmostLessState(Automata& a, char ch) {
	a.state = AutomataStates::CreateOperationState;
	if (ch != '>' and ch != '=')
		a.foreheadPos -= 1;
}


void Automata::AlmostGreaterState(Automata& a, char ch) {
	a.state = AutomataStates::CreateOperationState;
	if (ch != '=')
		a.foreheadPos -= 1;
}

void Automata::AlmostColonState(Automata& a, char ch) {
	a.state = AutomataStates::CreateOperationState;
	if (ch != '=')
		a.foreheadPos -= 1;
}

void Automata::StringCharConstState(Automata& a, char ch) {
	//warning
	if (ch == '\n') {
		a.state = AutomataStates::ErrorStringExceedsLine;
		return;
	}
	if (ch == '\'') {
		a.state = AutomataStates::CreateCharOrStringState;
		return;
	}
	a.state = AutomataStates::StringCharConstState;
}

void Automata::IntConstState(Automata& a, char ch) {
	if (checkIsNumeral(ch)) {
		a.state = AutomataStates::IntConstState;
		return;
	}
	if (ch == '.') {
		a.state = AutomataStates::AlmostFloatConstState;
		return;
	}
	a.foreheadPos -= 1;
	a.state = AutomataStates::CreateIntState;

}

void Automata::AlmostFloatConstState(Automata& a, char ch) {
	if (checkIsNumeral(ch)) {
		a.state = AutomataStates::FloatConstState;
		return;
	}
	a.foreheadPos -= 2;
	a.state = AutomataStates::CreateIntState;

}

void Automata::FloatConstState(Automata& a, char ch) {
	if (checkIsNumeral(ch)) {
		a.state = AutomataStates::FloatConstState;
		return;
	}
	a.foreheadPos -= 1;
	a.state = AutomataStates::CreateFloatState;
}

