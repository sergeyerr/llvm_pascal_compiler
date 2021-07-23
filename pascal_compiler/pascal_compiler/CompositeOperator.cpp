#include "Compiler.h"
#include "OperationToken.h";

ASTReturn Compiler::compositeOperator() {
	accept(EKeyWord::Begin);
	try {
		unmarkedOperator();
	}
	catch (IError* e) {
		if (e->Type == EErrorType::FailedToRecover) throw new FailedToRecoverError();
		saveError(e);
		bool processed = false;
		if (currentToken->Type == ETokenType::Operation) {
			auto token = static_cast<OperationToken*>(currentToken.get());
			if (token->Operation == EKeyWord::End || token->Operation == ESystemSymbol::Semicolon) {
				processed = true;
			}
		}
		if (!processed) {
			auto msg = lexer.ReadWhileNot([](IToken* t) {
				if (t->Type != ETokenType::Operation) return false;
				auto token = static_cast<OperationToken*>(t);
				if (token->Operation == EKeyWord::End || token->Operation == ESystemSymbol::Semicolon) return true;
				return false;
				});
			if (msg.EndOfFile)
				throw new FailedToRecoverError();
			else {
				next_token();
			}
		}
	}
	checkOperation();
	auto token = static_cast<OperationToken*>(currentToken.get());
	while (token->Operation == ESystemSymbol::Semicolon) {

		try {
			next_token();
			if (currentToken->Type == ETokenType::Operation) {
				token = static_cast<OperationToken*>(currentToken.get());
				if (token->Operation == EKeyWord::End) break;
			}
			unmarkedOperator();
			checkOperation();
			token = static_cast<OperationToken*>(currentToken.get());
		}
		catch (IError* e) {
			if (e->Type == EErrorType::FailedToRecover) throw new FailedToRecoverError();
			saveError(e);
			bool processed = false;
			if (currentToken->Type == ETokenType::Operation) {
				token = static_cast<OperationToken*>(currentToken.get());
				if (token->Operation == EKeyWord::End || token->Operation == ESystemSymbol::Semicolon) {
					processed = true;
					token = static_cast<OperationToken*>(currentToken.get());
				}
			}
			if (!processed) {
				auto msg = lexer.ReadWhileNot([](IToken* t) {
					if (t->Type != ETokenType::Operation) return false;
					auto token = static_cast<OperationToken*>(t);
					if (token->Operation == EKeyWord::End || token->Operation == ESystemSymbol::Semicolon) return true;
					return false;
					});
				if (msg.EndOfFile)
					throw new FailedToRecoverError();
				else {
					next_token();
					token = static_cast<OperationToken*>(currentToken.get());
				}
			}
		}
	}
	accept(EKeyWord::End, true);
	return ASTReturn{ nullptr };
}