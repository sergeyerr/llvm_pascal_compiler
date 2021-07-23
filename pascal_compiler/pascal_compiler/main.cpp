#include <iostream>
#include <vector>
#include "OperationEnumWrapper.h"
#include "IToken.h"
#include "ValueToken.h"
#include "OperationToken.h"
#include "IdentifierToken.h"
#include "LexicalAnalyzer.h"
#include "TokenOstream.h"
#include "Compiler.h";
#include <fstream>
#include "SemanticTable.h"


int main(int argc, char** argv) {
	//тесты
	//OperationEnumWrapper operation1(OpenCurlyBracket);
	//OperationEnumWrapper operation2(CloseCurlyBracket);
	//OperationEnumWrapper operation3(CloseCurlyBracket);
	//std::cout << (operation1 == operation2) << (operation2 == operation3) << '\n';
	//std::cout << (operation1.OperationType == OpenBracket) << '\n';

	//auto token_char = &ValueToken('c');
	//auto token_oper = &OperationToken(ESystemSymbol::Semicolon);
	//auto token_ident = &IdentifierToken("test_identificator");
	//std::vector<IToken*> arr;
	//arr.push_back(token_char);
	//arr.push_back(token_oper);
	//arr.push_back(token_ident);
	//auto back_char = static_cast<ValueToken*>(arr[0]);
	//auto back_oper = static_cast<OperationToken*>(arr[1]);
	//auto back_ident = static_cast<IdentifierToken*>(arr[2]);
	//std::cout << (token_char->Type == back_char->Type) << (token_oper->Operation.OperationType == back_oper->Operation.OperationType) \
	//	<< (token_ident->Name == back_ident->Name);


	//(не компилится)
	//operation1.Type = 2;
	//(интерфейсы не создаются)
	//auto test = IToken(ETokenType::Corrupted);


	//LexicalAnalyzer lexer("lexer_tests/hello_world.pas");
	//LexicalAnalyzer lexer("lexer_tests/invalid_symbol.pas");
	//LexicalAnalyzer lexer("lexer_tests/arithmetic.pas");
	//LexicalAnalyzer lexer("lexer_tests/floats.pas");
	//LexicalAnalyzer lexer("lexer_tests/variables.pas");
	//LexicalAnalyzer lexer("lexer_tests/tricky_comparisons.pas");
	//LexicalAnalyzer lexer("lexer_tests/assign_operator.pas");

	/*LexicalAnalyzer lexer("lexer_tests/wrong_float.pas");
	while (True) {
		auto msg = lexer.Read();
		if (msg.IsError) std::cout << "ERROR: " << msg.Error.ErrorType << '\n';
		if (msg.EndOfFile || msg.IsError) {
			std::cout << "END OF LEXER OUTPUT";
			break;
		}
		else {
			if (msg.Token->Type == ETokenType::Identifier) 
				std::cout << *(static_cast<IdentifierToken*>(msg.Token.get()))<< '\n';
			else if(msg.Token->Type == ETokenType::Operation)
				std::cout << *(static_cast<OperationToken*>(msg.Token.get())) << '\n';
			else std::cout << *(static_cast<ValueToken*>(msg.Token.get())) << '\n';
		}
	}*/
	std::string in_path = "semantic_tests/until_if_error.pas";
	std::string out_path = "out.exe";
	if (argc == 1) {
		std::cout << "using code constants for input and output files\n";
	}
	else {
		if (argc >= 2) in_path = argv[1];
		if (argc >= 3) out_path = argv[2];

	}
	Compiler comp(in_path, out_path);
	comp.Start();
	int res = 0;
	for (std::string s : comp.errorBus) {
		std::cout << s << "\n";
	}
	if (comp.IsParsed()) {
		std::cout << "Analysis successful\n" << "\n";
	}
	else {
		std::cout << "Analysis failed!\n" << "\n";
	}
	if (comp.IsCompilable() && comp.IsParsed()) {
		res = comp.Compile();
		if (res == 0) {
			std::string cmd = "gcc .\\tmp.obj -o " + out_path + " -lmsvcrt";// && rm tmp.obj";
			system(cmd.c_str());
			std::cout << "Executable generation succesfull\n" << "\n";
		}
		else {
			std::cout << "Executable generation failed!\n" << "\n";
		}
	}
	else {
		std::cout << "Codegen prevented\n" << "\n";
	}
	//std::ofstream out("log.txt");
	return res;
	//out.close();
}