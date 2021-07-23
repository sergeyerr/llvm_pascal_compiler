#include "Compiler.h"
#include "OperationToken.h"
#include "IdentifierToken.h"
ASTReturn Compiler::variableSection() {
	accept(EKeyWord::Var);
	do {
		try {
			// переменные одного типа
			std::vector<std::string> vars;
			if (currentToken->Type != ETokenType::Identifier) {
				throwSyntaxError("Expected identifier");
			}
			auto var_token = static_cast<IdentifierToken*>(currentToken.get());
			vars.push_back(var_token->Name);
			next_token();
			checkOperation();
			auto token = static_cast<OperationToken*>(currentToken.get());
			while (token->Operation == ESystemSymbol::Comma) {
				next_token();
				if (currentToken->Type != ETokenType::Identifier) {
					throwSyntaxError("Expected identifier");
				}
				var_token = static_cast<IdentifierToken*>(currentToken.get());
				vars.push_back(var_token->Name);
				next_token();
				checkOperation();
				token = static_cast<OperationToken*>(currentToken.get());
			}
			accept(ESystemSymbol::Colon);
			auto t = type().ReturnType;
			// костыличи только на базовые типы
			auto str_type = sem_table.GetBaseType("string");
			auto float_type = sem_table.GetBaseType("float");
			auto char_type = sem_table.GetBaseType("char");
			auto int_type = sem_table.GetBaseType("integer");
			auto bool_type = sem_table.GetBaseType("boolean");
			for (std::string name : vars) {
				sem_table.CreateIdentifier(name, EIdentifierUsage::Variable, t);
				// codegen
				if (t == str_type) {
					LLVMCodeGenerator.CreateVariable(name, EValueType::StringValue);
				}
				else if (t == int_type) {
					LLVMCodeGenerator.CreateVariable(name, EValueType::IntegerValue);
				}
				else if (t == float_type) {
					LLVMCodeGenerator.CreateVariable(name, EValueType::FloatValue);
				}
				else if (t == char_type) {
					LLVMCodeGenerator.CreateVariable(name, EValueType::CharValue);
				}
				else if (t == bool_type) {
					// пока что считаем bool за int, в дальнейшем мб переделать
					// alarm
					LLVMCodeGenerator.CreateVariable(name, EValueType::BoolValue);
				}
				else {
					is_compilable = false;
				}
				// end of codegen
			}
			accept(ESystemSymbol::Semicolon);
		}
		catch (IError* e) {
			if (e->Type == EErrorType::FailedToRecover) throw new FailedToRecoverError();
			saveError(e);
			bool processed = false;
			if (currentToken->Type == ETokenType::Operation) {
				auto token = static_cast<OperationToken*>(currentToken.get());
				if (token->Operation == ESystemSymbol::Semicolon) {
					processed = true;
					next_token();
				}
			}
			if (!processed) {
				auto msg = lexer.ReadWhileNot([](IToken* t) {
					if (t->Type != ETokenType::Operation) return false;
					auto token = static_cast<OperationToken*>(t);
					if (token->Operation == ESystemSymbol::Semicolon) return true;
					return false;
					});
				if (msg.EndOfFile)
					throw new FailedToRecoverError();
				else {
					// прочитали ;
					next_token();
					// прочитали identifier
					next_token();
				}
			}
		}

	} while (currentToken->Type == ETokenType::Identifier);
	return ASTReturn{ nullptr };
}