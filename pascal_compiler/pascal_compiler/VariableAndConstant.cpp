#include "Compiler.h"
#include "OperationToken.h"
#include "ValueToken.h"
#include "SemanticError.h"
#include "IdentifierToken.h"

ASTReturn Compiler::constant() {
	if (currentToken->Type == ETokenType::Operation) {
		auto token = static_cast<OperationToken*>(currentToken.get());
		// сюда кодген не забыть
		if (token->Operation == EAdditiveOperation::Plus) {
			accept(EAdditiveOperation::Plus);
		}
		else if (token->Operation == EAdditiveOperation::Minus) {
			accept(EAdditiveOperation::Minus);
		}
		else throwSyntaxError("expected sign of constant");


		// семантическая проверка на число!!!
		if (currentToken->Type == ETokenType::Value) {
			auto token = static_cast<ValueToken*>(currentToken.get());
			if (token->Variant->Type != EValueType::IntegerValue && token->Variant->Type != EValueType::FloatValue) {
				throw new SemanticError("expected numeric constant");
			}
			next_token();
			// semantic part
			if (token->Variant->Type == EValueType::IntegerValue) {
				auto tmp = static_cast<IntVariant*>(token->Variant);
				return ASTReturn{ sem_table.GetBaseType("integer") , tmp->IntValue, LLVMCodeGenerator.CreateConst(token->Variant) };
			}
			else {
				//auto tmp = static_cast<FloatVariant*>(token->Variant);
				return ASTReturn{ sem_table.GetBaseType("float"), -1, LLVMCodeGenerator.CreateConst(token->Variant) };
			}
			// end of semantic part
		}
		else if (currentToken->Type == ETokenType::Identifier) {
			auto token_name = static_cast<IdentifierToken*>(currentToken.get())->Name;
			auto token_ident = sem_table.AccessIdentifier(token_name);
			if (token_ident->Usage != EIdentifierUsage::Const) {
				throw new SemanticError("tried to access variable as a const");
			}
			next_token();
			return ASTReturn{ token_ident->Type, -1, LLVMCodeGenerator.AccessNamedConst(token_name) };
		}
		else {
			throwSyntaxError("expected constant token");
		}

	}
	else {
		return constantNoSign();
	}
}


ASTReturn Compiler::constantNoSign() {
	if (currentToken->Type == ETokenType::Operation) {
		accept(EKeyWord::Nil);
		// на всякий случай, будем считать, что это 0
		return ASTReturn{ sem_table.GetBaseType("integer"), 0, LLVMCodeGenerator.CreateConst(new IntVariant(0)) };
	}
	else if (currentToken->Type == ETokenType::Value) {
		// semantic part
		auto token = static_cast<ValueToken*>(currentToken.get());
		switch (token->Variant->Type) {
		case EValueType::IntegerValue: {
			auto tmp1 = static_cast<IntVariant*>(token->Variant);
			auto res = ASTReturn{ sem_table.GetBaseType("integer") , tmp1->IntValue, LLVMCodeGenerator.CreateConst(token->Variant) };
			next_token();
			return res;
		}
		case EValueType::FloatValue: {
			//auto tmp2 = static_cast<FloatVariant*>(token->Variant);
			auto res =  ASTReturn{ sem_table.GetBaseType("float"), -1, LLVMCodeGenerator.CreateConst(token->Variant) };
			next_token();
			return res;
		}
		case EValueType::CharValue: {
			auto tmp3 = static_cast<CharVariant*>(token->Variant);
			auto res = ASTReturn{ sem_table.GetBaseType("char") , tmp3->CharValue, LLVMCodeGenerator.CreateConst(token->Variant) };
			next_token();
			return res;
		}
		case EValueType::StringValue:
			auto res = ASTReturn{ sem_table.GetBaseType("string") , -1, LLVMCodeGenerator.CreateConst(token->Variant) };
			next_token();
			return res;
		}
		// end of semantic part
	}
	else {
		auto token_name = static_cast<IdentifierToken*>(currentToken.get())->Name;
		auto token_ident = sem_table.AccessIdentifier(token_name);
		if (token_ident->Usage != EIdentifierUsage::Const) {
			throw new SemanticError("tried to access variable as a const");
		}
		next_token();
		return ASTReturn{token_ident->Type, -1, LLVMCodeGenerator.AccessNamedConst(token_name) };
	}
}

ASTReturn Compiler::variable() {
	std::string var_name;
	if (currentToken->Type == ETokenType::Identifier) {
		auto token = static_cast<IdentifierToken*>(currentToken.get());
		//sem part
		auto token_ident = sem_table.AccessIdentifier(token->Name);
		if (token_ident->Usage != EIdentifierUsage::Variable) {
			throw new SemanticError("tried to access constant as a variable");
		}
		//end of sem part
		var_name = token->Name;
		next_token();
		if (currentToken->Type == ETokenType::Operation) {
			//проверка на индексированнуую переменная
			auto token = static_cast<OperationToken*>(currentToken.get());

			// semantic and syntax part
			std::vector<IType*> indexTypes;
			if (token->Operation == EOpenBracket::OpenSquareBracket) {
				// доступ к массиву
				// prevent_compile
				is_compilable = false;
				next_token();
				indexTypes.push_back(expression().ReturnType);
				checkOperation();
				auto token = static_cast<OperationToken*>(currentToken.get());
				while (token->Operation == ESystemSymbol::Comma) {
					accept(ESystemSymbol::Comma);
					indexTypes.push_back(expression().ReturnType);
					token = static_cast<OperationToken*>(currentToken.get());
				}
				accept(ECloseBracket::CloseSquareBracket);
				return ASTReturn{ sem_table.AccessArray(var_name, indexTypes) };
			}
			else {
				auto token_ident = sem_table.AccessIdentifier(var_name);
				if (token_ident->Usage != EIdentifierUsage::Variable) {
					throw new SemanticError("tried to access constant as a variable");
				}
				// тут можно словить ошибку каста в LLVM
				return ASTReturn{ token_ident->Type, -1, LLVMCodeGenerator.AccessVariable(var_name), LLVMCodeGenerator.GetVariable(var_name) };
			}
			// end of semantic and syntax part
		}
		else {
			//sem part
			auto token_ident = sem_table.AccessIdentifier(var_name);
			if (token_ident->Usage != EIdentifierUsage::Variable) {
				throw new SemanticError("tried to access constant as a variable");
			}
			// тут можно словить ошибку каста в LLVM
			return ASTReturn{ token_ident->Type, -1, LLVMCodeGenerator.AccessVariable(var_name), LLVMCodeGenerator.GetVariable(var_name) };
		}
	}
	else {
		throwSyntaxError("identifier expected");
	}
}