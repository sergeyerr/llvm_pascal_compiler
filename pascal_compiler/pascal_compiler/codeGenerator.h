#pragma once
#include "llvm/ADT/APFloat.h"
#include "llvm/ADT/APInt.h";
#include "llvm/ADT/Optional.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Verifier.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/Host.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/TargetRegistry.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Target/TargetOptions.h"
#include <algorithm>
#include <cassert>
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <map>
#include <memory>
#include <string>
#include <system_error>
#include <utility>
#include <vector>
#include <vector>
#include <string>
#include "EValueType.h"
#include "IVariant.h"
#include "IntVariant.h"
#include "StringVariant.h"
#include "CharVariant.h"
#include "FloatVariant.h"


using namespace llvm;
using namespace llvm::sys;


class CodeGenerator {
public:
    llvm::LLVMContext* context;
    llvm::Module* module;
    llvm::IRBuilder<>* builder;
    std::map<std::string, AllocaInst*> Variables;
    std::map<std::string, AllocaInst*> Consts;
    llvm::Constant* printfPointer;


    CodeGenerator(bool* compile);
    ~CodeGenerator();
    int GenerateCode(std::string output);
    void Printf(std::vector<Value*> vals);
    bool* compile;

    // создаём переменную в мейне
    void createBoolConsts();
    AllocaInst* CreateEntryBlockAlloca(llvm::Function* TheFunction,
        const std::string& VarName, EValueType valueType);

    AllocaInst* CreateVariable(const std::string& VarName, EValueType valueType);
    Value* AccessVariable(const std::string& VarName);
    AllocaInst* GetVariable(const std::string& VarName);
    //Value* CreateNamedConst(const std::string& VarName, IVariant* var);
    Value* AccessNamedConst(const std::string& VarName);
    void StoreInVariable(const std::string& VarName, Value* val);
    void StoreInVariable(AllocaInst* var, Value* val);
    Value* CreateConst(IVariant* var);
    Value* Plus(Value* a, Value* b, EValueType valType);
    Value* Minus(Value* a, Value* b, EValueType valType);
    Value* Mult(Value* a, Value* b, EValueType valType);
    Value* Divide(Value* a, Value* b, EValueType valType);
    Value* MinusConst(Value* a, EValueType valType);

};