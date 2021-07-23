#pragma once
#include "IType.h"
#include "llvm/IR/Value.h";
#include "llvm/ADT/APFloat.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Verifier.h"

struct ASTReturn {
	IType* ReturnType;
	// используется для констант и ренджей
	int val = -1;
	llvm::Value* llvm_val = nullptr;
	llvm::AllocaInst* llvm_var = nullptr;
};