#include "Compiler.h"
#include "OperationToken.h"

ASTReturn Compiler::block() {
	checkOperation();
	auto token = static_cast<OperationToken*>(currentToken.get());
	while (token->Operation != EKeyWord::Begin) {
		try {
			switch (token->Operation)
			{
			case EKeyWord::Var:
				variableSection();
				break;
			case EKeyWord::Type:
				typesSection();
				break;
			default:
				throwSyntaxError("Unexpected keyword in general block");
			}
			token = static_cast<OperationToken*>(currentToken.get());
		}
		catch (IError* e) {
			if (e->Type == EErrorType::FailedToRecover) throw new FailedToRecoverError();
			saveError(e);
			// на случай, если текущий токен - тот, на который мы восстанавливаемся
			bool processed = false;
			if (currentToken->Type == ETokenType::Operation) {
				token = static_cast<OperationToken*>(currentToken.get());
				if (token->Operation == EKeyWord::Begin || token->Operation == ESystemSymbol::Semicolon) {
					processed = true;
					if (token->Operation == ESystemSymbol::Semicolon) {
						accept(ESystemSymbol::Semicolon, true);
						token = static_cast<OperationToken*>(currentToken.get());
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
					throw new FailedToRecoverError();
				else {
					next_token();
					checkOperation();
					token = static_cast<OperationToken*>(currentToken.get());
					if (token->Operation == ESystemSymbol::Semicolon) {
						accept(ESystemSymbol::Semicolon);
						token = static_cast<OperationToken*>(currentToken.get());
					}
				}
			}
		}
	}

	compositeOperator();
	return ASTReturn{ nullptr };

}