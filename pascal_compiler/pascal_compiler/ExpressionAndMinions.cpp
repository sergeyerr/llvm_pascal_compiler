#include "Compiler.h"
#include "OperationToken.h"
#include "IdentifierToken.h"
#include "OperationEnumWrapper.h"
#include "SemanticError.h"


ASTReturn Compiler::expression() {
	auto res = simpleExpression();
	auto res_type = res.ReturnType;
	auto codegen = res.llvm_val;
	if (currentToken->Type == ETokenType::Operation) {
		auto token = static_cast<OperationToken*>(currentToken.get());
		if (token->Operation.OperationType == EOperationType::ComparisonOperation) {
			// prevent_compile
			is_compilable = false;
			next_token();
			res_type = sem_table.CompareOperation(res_type, simpleExpression().ReturnType);
		}
	}
	return ASTReturn{ res_type, -1, codegen };
}


ASTReturn Compiler::simpleExpression() {
	bool needs_minus = false;
	if (currentToken->Type == ETokenType::Operation) {
		auto token = static_cast<OperationToken*>(currentToken.get());
		if (token->Operation == EAdditiveOperation::Minus || token->Operation == EAdditiveOperation::Plus) {
			if (token->Operation == EAdditiveOperation::Plus) {
				needs_minus = true;
			}
			next_token();
		}
	}
	auto ret = term();
	auto res_type = ret.ReturnType;
	llvm::Value* codegen = ret.llvm_val;
	if (needs_minus) {
		//codegen
		if (res_type != sem_table.GetBaseType("float") || sem_table.GetBaseType("integer")) throw new SemanticError("expected int or  float type in expr");
		EValueType code_gen_type = EValueType::IntegerValue;
		if (res_type == sem_table.GetBaseType("float")) code_gen_type = EValueType::FloatValue;
		codegen = LLVMCodeGenerator.MinusConst(codegen, code_gen_type);
	}
	while (currentToken->Type == ETokenType::Operation) {
		auto token = static_cast<OperationToken*>(currentToken.get());
		if (token->Operation.OperationType == EOperationType::AdditiveOperation) {
			auto oper = token->Operation;
			next_token();
			ret = term();
			res_type = sem_table.ArithmeticOperation(res_type, ret.ReturnType);
			EValueType code_gen_type = EValueType::IntegerValue;
			if (res_type == sem_table.GetBaseType("float")) code_gen_type = EValueType::FloatValue;
			//codegen
			switch (oper)
			{
			case EAdditiveOperation::Minus:
				codegen = LLVMCodeGenerator.Minus(codegen, ret.llvm_val, code_gen_type);
				break;
			case EAdditiveOperation::Plus:
				codegen = LLVMCodeGenerator.Plus(codegen, ret.llvm_val, code_gen_type);
				break;
			default:
				is_compilable = false;
				break;
			}

			//end of code gen
		}
		else break;
	}
	return ASTReturn{ res_type, -1, codegen };
}



ASTReturn Compiler::term() {
	auto ret = mult();
	auto res_type = ret.ReturnType;
	llvm::Value* codegen = ret.llvm_val;

	while (currentToken->Type == ETokenType::Operation) {
		auto token = static_cast<OperationToken*>(currentToken.get());
		if (token->Operation.OperationType == EOperationType::MultiplicativeOperation) {
			auto oper = token->Operation;
			next_token();
			ret = mult();
			res_type = sem_table.ArithmeticOperation(res_type, ret.ReturnType);
			EValueType code_gen_type = EValueType::IntegerValue;
			if (res_type == sem_table.GetBaseType("float")) code_gen_type = EValueType::FloatValue;
			//codegen
			switch (oper)
			{
			case EMultiplicativeOperation::Divide:
				codegen = LLVMCodeGenerator.Divide(codegen, ret.llvm_val, code_gen_type);
				break;
			case EMultiplicativeOperation::Multiply:
				codegen = LLVMCodeGenerator.Mult(codegen, ret.llvm_val, code_gen_type);
				break;
			default:
				is_compilable = false;
				break;
			}

			//end of code gen
		}
		else break;
	}
	return ASTReturn{ res_type, -1, codegen };
}

ASTReturn Compiler::mult() {
	if (currentToken->Type == ETokenType::Operation) {
		auto token = static_cast<OperationToken*>(currentToken.get());
		if (token->Operation == EOpenBracket::OpenRoundBracket) {
			next_token(); 
			auto tmp = expression();
			accept(ECloseBracket::CloseRoundBracket);
			return tmp;
		}
		else if (token->Operation == EKeyWord::Not) {
			next_token();
			return mult();
		}
		else {
			//nil;
			return constantNoSign();
		}
	}
	// тут обработка переменной и константы
	else {
		if (currentToken->Type == ETokenType::Value) {
			return constantNoSign();
		}
		else {
			auto ident_token = static_cast<IdentifierToken*>(currentToken.get());
			auto ident = sem_table.AccessIdentifier(ident_token->Name);
			//sem part
			if (ident->Usage == EIdentifierUsage::Const) {
				return constantNoSign();
			}
			else {
				return variable();
			}
		}
	}
}