#include "Compiler.h"
#include "OperationToken.h"
ASTReturn Compiler::program() {

	try {
		accept(EKeyWord::Program);
		if (currentToken->Type != ETokenType::Identifier) {
			throwSyntaxError("Expected program name");
		}
		next_token();
		if (currentToken->Type == ETokenType::Operation && (static_cast<OperationToken*>(currentToken.get())->Operation == EOpenBracket::OpenRoundBracket)) {
			accept(EOpenBracket::OpenRoundBracket);
			if (currentToken->Type != ETokenType::Identifier) {
				throwSyntaxError("Expected program name");
			}
			next_token();
			checkOperation();
			auto token = static_cast<OperationToken*>(currentToken.get());
			while (token->Operation != ECloseBracket::CloseRoundBracket) {
				accept(ESystemSymbol::Comma);
				if (currentToken->Type != ETokenType::Identifier) {
					throwSyntaxError("Expected program name");
				}
				next_token();
				token = static_cast<OperationToken*>(currentToken.get());
			}
			accept(ECloseBracket::CloseRoundBracket);
		}
		accept(ESystemSymbol::Semicolon);
	}
	catch (IError* e) {
		if (e->Type == EErrorType::FailedToRecover) return ASTReturn{ nullptr };
		saveError(e);
		// на случай, если текущий токен - тот, на который мы восстанавливаемся
		bool processed = false;
		if (currentToken->Type == ETokenType::Operation) {
			auto token = static_cast<OperationToken*>(currentToken.get());
			if (token->Operation == EKeyWord::Begin || token->Operation == ESystemSymbol::Semicolon) {
				processed = true;
				auto token = static_cast<OperationToken*>(currentToken.get());
				if (token->Operation == ESystemSymbol::Semicolon) {
					accept(ESystemSymbol::Semicolon, true);
				}
			}
		}
		if (!processed) {
			auto msg = lexer.ReadWhileNot([](IToken* t) {
				if (t->Type != ETokenType::Operation) return false;
				auto token = static_cast<OperationToken*>(t);
				if (token->Operation == EKeyWord::Begin || token->Operation == ESystemSymbol::Semicolon) return true;
				return false;
				});
			if (msg.EndOfFile)
				// не нашли бегин и умерли
				return ASTReturn{ nullptr };
			else {
				next_token();
				checkOperation();
				auto token = static_cast<OperationToken*>(currentToken.get());
				if (token->Operation == ESystemSymbol::Semicolon) {
					accept(ESystemSymbol::Semicolon, true);
				}
			}
		}
	}
// тут можно починить ошибку
	try {
		block();
	}
	catch (IError* e) {
		if (e->Type != EErrorType::FailedToRecover) saveError(e);
		analyzed = false;
	}
	return ASTReturn{ nullptr };

}