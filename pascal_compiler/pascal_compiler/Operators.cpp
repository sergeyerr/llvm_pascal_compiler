#include "Compiler.h"
#include "OperationToken.h"
#include "IdentifierToken.h";
#include "SemanticError.h"
// без меток на операторы
ASTReturn Compiler::unmarkedOperator() {
	if (currentToken->Type == ETokenType::Operation) {
		// сложный оператор
		auto token = static_cast<OperationToken*>(currentToken.get());
		switch (token->Operation) {
		case EKeyWord::If:
			ifOperator();
			break;
		case EKeyWord::While:
			whileOperator();
			break;
		case EKeyWord::Begin:
			compositeOperator();
			break;
		case EKeyWord::Repeat:
			repeatOperator();
			break;
		case EKeyWord::For:
			forOperator();
			break;
		default:
			throwSyntaxError("Expected some complex operator, like 'if', or simple operator");
		}
	}
	// как бы любой простой оператор, но из простых операторов нужно только assign
	else {
		// тут может быть процедура, на случай того, если принтить захотим
		if (currentToken.get()->Type != ETokenType::Identifier) {
			throwSyntaxError("Expected identifier");
		}
		auto token = static_cast<IdentifierToken*>(currentToken.get());
		auto ident = sem_table.AccessIdentifier(token->Name);
		// костыльная процедура aka вызов плюсового printf
		if (ident->Usage == EIdentifierUsage::Procedure) {
			next_token();
			std::vector<IType*> args_types;
			std::vector<llvm::Value*> args_payload;
			accept(EOpenBracket::OpenRoundBracket);
			auto ret = expression();
			args_types.push_back(ret.ReturnType);
			args_payload.push_back(ret.llvm_val);
			checkOperation();
			auto operToken = static_cast<OperationToken*>(currentToken.get());
			while (operToken->Operation == ESystemSymbol::Comma) {
				next_token();
				ret = expression();
				args_types.push_back(ret.ReturnType);
				args_payload.push_back(ret.llvm_val);
				checkOperation();
				operToken = static_cast<OperationToken*>(currentToken.get());

			}
			accept(ECloseBracket::CloseRoundBracket);
			sem_table.Printf(args_types);
			LLVMCodeGenerator.Printf(args_payload);

		}
		else 
			assignOper();
	}
	return ASTReturn{ nullptr };
}


// без присваивания функции(из книжки Зологовой)
ASTReturn Compiler::assignOper() {
	auto ret = variable();
	auto var_type = ret.ReturnType;
	auto codegen = ret.llvm_var;
	accept(ESystemSymbol::Assign);
	auto expr = expression();
	sem_table.AssignVar(var_type, expr.ReturnType);
	LLVMCodeGenerator.StoreInVariable(codegen, expr.llvm_val);
	return ASTReturn{ var_type};
}

ASTReturn Compiler::ifOperator() {
	// prevent_compile
	is_compilable = false;
	accept(EKeyWord::If);
	try {
		auto expr = expression().ReturnType;
		if (expr != sem_table.GetBaseType("boolean")) throw new SemanticError("boolean type expression is needed for condition");
	}
	catch (IError* e) {
		if (e->Type == EErrorType::FailedToRecover) throw new FailedToRecoverError();
		saveError(e);
		bool processed = false;
		// на случай, если текущий токен - тот, на который мы восстанавливаемся
		if (currentToken->Type == ETokenType::Operation) {
			auto token = static_cast<OperationToken*>(currentToken.get());
			if (token->Operation == EKeyWord::Then) processed = true;
		}
		if (!processed) {
			auto msg = lexer.ReadWhileNot([](IToken* t) {
				if (t->Type != ETokenType::Operation) return false;
				auto token = static_cast<OperationToken*>(t);
				if (token->Operation == EKeyWord::Then) return true;
				return false;
				});
			if (msg.EndOfFile)
				throw new FailedToRecoverError();
			else {
				next_token();
			}
		}
	}
	accept(EKeyWord::Then);
	// после then лучше не ловить ошибку, этим займётся CompositeOperator
	unmarkedOperator();
	if (currentToken->Type == ETokenType::Operation) {
		auto token = static_cast<OperationToken*>(currentToken.get());
		if (token->Operation == EKeyWord::Else) {
			next_token();
			unmarkedOperator();
		}
	}

	return ASTReturn{ nullptr };
}


ASTReturn Compiler::whileOperator() {
	// prevent_compile
	is_compilable = false;
	accept(EKeyWord::While);
	try {
		auto expr = expression().ReturnType;
		if (expr != sem_table.GetBaseType("boolean")) throw new SemanticError("boolean type expression is needed for condition");
	}
	catch (IError* e) {
		if (e->Type == EErrorType::FailedToRecover) throw new FailedToRecoverError();
		saveError(e);
		bool processed = false;
// на случай, если текущий токен - тот, на который мы восстанавливаемся
		if (currentToken->Type == ETokenType::Operation) {
			auto token = static_cast<OperationToken*>(currentToken.get());
			if (token->Operation == EKeyWord::Do) processed = true;
		}
		if (!processed) {
			auto msg = lexer.ReadWhileNot([](IToken* t) {
				if (t->Type != ETokenType::Operation) return false;
				auto token = static_cast<OperationToken*>(t);
				if (token->Operation == EKeyWord::Do) return true;
				return false;
				});
			if (msg.EndOfFile)
				throw new FailedToRecoverError();
			else {
				next_token();
			}
		}
	}
	accept(EKeyWord::Do);
	unmarkedOperator();
	return ASTReturn{ nullptr };
}

ASTReturn Compiler::repeatOperator() {
	// prevent_compile
	is_compilable = false;
	accept(EKeyWord::Repeat);
	unmarkedOperator();
	auto token = static_cast<OperationToken*>(currentToken.get());
	while (token->Operation == ESystemSymbol::Semicolon) {
		try {
			accept(ESystemSymbol::Semicolon);
			unmarkedOperator();
			token = static_cast<OperationToken*>(currentToken.get());
		}
		catch (IError* e) {
			if (e->Type == EErrorType::FailedToRecover) throw new FailedToRecoverError();
			saveError(e);
			bool processed = false;
			// на случай, если текущий токен - тот, на который мы восстанавливаемся
			if (currentToken->Type == ETokenType::Operation) {
				token = static_cast<OperationToken*>(currentToken.get());
				if (token->Operation == EKeyWord::Until || token->Operation == ESystemSymbol::Semicolon) {
					processed = true;
				}
			}
			if (!processed) {
				auto msg = lexer.ReadWhileNot([](IToken* t) {
					if (t->Type != ETokenType::Operation) return false;
					auto token = static_cast<OperationToken*>(t);
					if (token->Operation == EKeyWord::Until || token->Operation == ESystemSymbol::Semicolon) return true;
					return false;
					});
				if (msg.EndOfFile)
					// не нашли бегин и умерли
					throw new FailedToRecoverError();
				else {
					next_token();
				}
			}
		}
	}
	accept(EKeyWord::Until);
	auto expr = expression().ReturnType;
	if (expr != sem_table.GetBaseType("boolean")) throw new SemanticError("boolean type expression is needed for condition");
}

ASTReturn Compiler::forOperator() {
	// prevent_compile
	is_compilable = false;
	accept(EKeyWord::For);
	IType* iterable_type = nullptr;
	if (currentToken->Type != ETokenType::Identifier) {
		throwSyntaxError("identifier expected");
	}
	else {
		try {
			iterable_type = assignOper().ReturnType;
		}
		catch (IError* e) {
			if (e->Type == EErrorType::FailedToRecover) throw new FailedToRecoverError();
			saveError(e);
			bool processed = false;
			// на случай, если текущий токен - тот, на который мы восстанавливаемся
			if (currentToken->Type == ETokenType::Operation) {
				auto token = static_cast<OperationToken*>(currentToken.get());
				if (token->Operation == EKeyWord::Downto || token->Operation == EKeyWord::To) {
					processed = true;
				}
			}
			if (!processed) {
				auto msg = lexer.ReadWhileNot([](IToken* t) {
					if (t->Type != ETokenType::Operation) return false;
					auto token = static_cast<OperationToken*>(t);
					if (token->Operation == EKeyWord::Downto || token->Operation == EKeyWord::To) return true;
					return false;
					});
				if (msg.EndOfFile)
					throw new FailedToRecoverError();
				else {
					next_token();
				}
			}
		}
		auto token = static_cast<OperationToken*>(currentToken.get());
		if (token->Operation == EKeyWord::Downto) {
			accept(EKeyWord::Downto);
		}
		else if (token->Operation == EKeyWord::To) {
			accept(EKeyWord::To);
		}
		else {
			throwSyntaxError("expected downto or to");
		}
		auto expr_type = expression().ReturnType;
		if (iterable_type != nullptr && iterable_type != expr_type) {
			throw new SemanticError("final target in for has incompatible type with iterable");
		}
		accept(EKeyWord::Do);
		unmarkedOperator();
		return ASTReturn{ nullptr };
	}
}