#include "Compiler.h"
#include "OperationToken.h"
#include "IdentifierToken.h"
#include "SemanticError.h"


ASTReturn Compiler::typesSection() {
	// prevent_compile
	is_compilable = false;
	accept(EKeyWord::Type);
	typeDefinition();
	accept(ESystemSymbol::Semicolon);
	while (currentToken->Type == ETokenType::Identifier) {
		typeDefinition();
		accept(ESystemSymbol::Semicolon);
	}
	return ASTReturn{ nullptr };
}


ASTReturn Compiler::typeDefinition() {
	if (currentToken->Type != ETokenType::Identifier) {
		throwSyntaxError("expected identifier");
	}
	auto token_name = static_cast<IdentifierToken*>(currentToken.get())->Name;
	//обработка идентификатора
	next_token();
	accept(EComparisonOperation::Equal);
	auto t = type().ReturnType;
	sem_table.CreateIdentifier(token_name, EIdentifierUsage::Type, t);
	return ASTReturn{ nullptr };
}

// забываем про составные и ссылочные типы
ASTReturn Compiler::type() {
	if (currentToken->Type == ETokenType::Operation) {
		auto token = static_cast<OperationToken*>(currentToken.get());
		if (token->Operation == EKeyWord::Array) {
			return arrayType();
		}
		else {
			return simpleType();
		}
	}
	else {
		return simpleType();;
	}
}


ASTReturn Compiler::simpleType() {
	if (currentToken->Type == ETokenType::Identifier) {
		// использование существующего типа
		auto token = static_cast<IdentifierToken*>(currentToken.get());

		//sem part
		auto t = sem_table.GetType(token->Name);
		next_token();
		return ASTReturn{ t };
		//end of sem part


	}
	else if (currentToken->Type == ETokenType::Operation) {
		auto token = static_cast<OperationToken*>(currentToken.get());
		//if (token->Operation.OperationType == EOperationType::TypeWord) {
		//	// костыль!! тоже типа существующих тип
		//	next_token();
		//}
		if (token->Operation != EOpenBracket::OpenRoundBracket) {
			//константа со знака может начинаться
			return intervalType();
		}
		else {
			return enumerableType();
		}
	}
	else {
		return intervalType();
	}
}


ASTReturn Compiler::enumerableType() {
	std::vector<std::string> names;
	accept(EOpenBracket::OpenRoundBracket);
	if (currentToken->Type == ETokenType::Identifier) {
		//semantic part
		auto token = static_cast<IdentifierToken*>(currentToken.get());
		names.push_back(token->Name);
		//end of sem part
		next_token();
	}
	else throwSyntaxError("expected identifier");
	checkOperation();
	auto token = static_cast<OperationToken*>(currentToken.get());
	while (token->Operation == ESystemSymbol::Comma) {
		accept(ESystemSymbol::Comma);
		if (currentToken->Type == ETokenType::Identifier) {
			//semantic part
			auto token = static_cast<IdentifierToken*>(currentToken.get());
			names.push_back(token->Name);
			//end of sem part
			next_token();
		}
		else throwSyntaxError("expected identifier");
		token = static_cast<OperationToken*>(currentToken.get());
	}
	accept(ECloseBracket::CloseRoundBracket);
	//semantic part
	auto t = sem_table.CreateEnumType(names);
	return ASTReturn{ t };
}


ASTReturn Compiler::intervalType() {
	auto min = constant();
	accept(ESystemSymbol::Range);
	auto max = constant();
	if (min.ReturnType != max.ReturnType) {
		throw new SemanticError("consts for range must have same type");
	}
	return ASTReturn{ sem_table.CreateRangeType(min.val, max.val, min.ReturnType) };
}

ASTReturn Compiler::arrayType() {
	accept(EKeyWord::Array);
	accept(EOpenBracket::OpenSquareBracket);
	std::vector<IType*> indecies;
	indecies.push_back(simpleType().ReturnType);
	checkOperation();
	auto token = static_cast<OperationToken*>(currentToken.get());
	while (token->Operation == ESystemSymbol::Comma) {
		accept(ESystemSymbol::Comma);
		indecies.push_back(simpleType().ReturnType);
		checkOperation();
		token = static_cast<OperationToken*>(currentToken.get());
	}
	accept(ECloseBracket::CloseSquareBracket);
	accept(EKeyWord::Of);
	auto t = type().ReturnType;
	return ASTReturn{ sem_table.CreateArrayType(t, indecies) };
}