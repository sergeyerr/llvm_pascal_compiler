#include "codeGenerator.h"

CodeGenerator::CodeGenerator(bool* c) {
    compile = c;
    context = new llvm::LLVMContext ();
    module = new llvm::Module("pascal_program", *context);
    builder = new llvm::IRBuilder<>(*context);

    // создаём void main
    llvm::FunctionType* funcType = llvm::FunctionType::get(builder->getVoidTy(), false);
    llvm::Function* mainFunc =
        llvm::Function::Create(funcType, llvm::Function::ExternalLinkage, "main", module);
    llvm::BasicBlock* entry = llvm::BasicBlock::Create(*context, "entrypoint", mainFunc);
    // переводим вставляемые команды в main
    builder->SetInsertPoint(entry);

    // инициализируем printf
    std::vector<llvm::Type*> printfArgs;
    printfArgs.push_back(builder->getInt8Ty()->getPointerTo());
    llvm::ArrayRef<llvm::Type*>  argsRef(printfArgs);
    llvm::FunctionType* printfType =
        llvm::FunctionType::get(builder->getInt32Ty(), argsRef, true);
    printfPointer = module->getOrInsertFunction("printf", printfType);
}

CodeGenerator::~CodeGenerator() {
   
    //падает, если попытаться удалить... В официальном туторе нигде нет удалений указателей, подозреваю, что там фабрика сидит где-то
    //delete module;
    //delete context;
    //delete builder;
}


void CodeGenerator::Printf(std::vector<Value*> vals) {
    if (!*compile) return;
    llvm::ArrayRef<llvm::Value*> payload(vals);
    builder->CreateCall(printfPointer, payload);
}


int CodeGenerator::GenerateCode(std::string Filename) {
    if (!*compile) return 1;
    // main возвращает void
    builder->CreateRetVoid();
    module->print(errs(), nullptr);
    // Initialize the target registry etc.
    InitializeAllTargetInfos();
    InitializeAllTargets();
    InitializeAllTargetMCs();
    InitializeAllAsmParsers();
    InitializeAllAsmPrinters();
    auto TargetTriple = sys::getDefaultTargetTriple();
    module->setTargetTriple(TargetTriple);

    std::string Error;
    auto Target = TargetRegistry::lookupTarget(TargetTriple, Error);

    // Print an error and exit if we couldn't find the requested target.
    // This generally occurs if we've forgotten to initialise the
    // TargetRegistry or we have a bogus target triple.
    if (!Target) {
        errs() << Error;
        return 1;
    }

    auto CPU = "generic";
    auto Features = "";

    TargetOptions opt;
    auto RM = Optional<Reloc::Model>();
    auto TheTargetMachine =
        Target->createTargetMachine(TargetTriple, CPU, Features, opt, RM);

    module->setDataLayout(TheTargetMachine->createDataLayout());

    std::error_code EC;
    raw_fd_ostream dest(Filename, EC, sys::fs::OpenFlags::F_None);

    if (EC) {
        errs() << "Could not open file: " << EC.message();
        return 1;
    }

    legacy::PassManager pass;
    auto FileType = TargetMachine::CGFT_ObjectFile;

    if (TheTargetMachine->addPassesToEmitFile(pass, dest, FileType)) {
        errs() << "TheTargetMachine can't emit a file of this type";
        return 1;
    }

    pass.run(*module);
    dest.flush();

    errs() << "Wrote object code to:" << Filename << "\n";

    return 0;
}



AllocaInst* CodeGenerator::CreateEntryBlockAlloca(llvm::Function* TheFunction,
    const std::string& VarName, EValueType valueType) {
    if (!*compile) return nullptr;
    IRBuilder<> TmpB(&TheFunction->getEntryBlock(),
        TheFunction->getEntryBlock().begin());
    switch (valueType) {
    case EValueType::FloatValue:
        return TmpB.CreateAlloca(Type::getDoubleTy(*context), 0,
            VarName.c_str());
    case EValueType::IntegerValue:
        return TmpB.CreateAlloca(Type::getInt32Ty(*context), 0,
            VarName.c_str());
    case EValueType::BoolValue:
        return TmpB.CreateAlloca(Type::getInt1Ty(*context), 0,
            VarName.c_str());
    case EValueType::StringValue:
        return TmpB.CreateAlloca(Type::getInt8PtrTy(*context), 0,
            VarName.c_str());
    case EValueType::CharValue:
        return TmpB.CreateAlloca(Type::getInt8Ty(*context), 0,
            VarName.c_str());
    }
}


AllocaInst* CodeGenerator::CreateVariable(const std::string& VarName, EValueType valueType) {
    if (!compile) return nullptr;
    Variables[VarName] = CreateEntryBlockAlloca(builder->GetInsertBlock()->getParent(), VarName, valueType);
    return Variables[VarName];
}


//Value* CodeGenerator::CreateNamedConst(const std::string& name, IVariant* var) {
//    Consts[name] = CreateEntryBlockAlloca(builder->GetInsertBlock()->getParent(), name, var->Type);
//    auto tmp = CreateConst(var);
//    Consts[name] = tmp;
//    return Consts[name];
//}


Value* CodeGenerator::AccessNamedConst(const std::string& name) {
    if (!*compile) return nullptr;
    return builder->CreateLoad(Consts[name], name.c_str());
}



Value* CodeGenerator::AccessVariable(const std::string& VarName) {
    if (!*compile) return nullptr;
    return builder->CreateLoad(Variables[VarName], VarName.c_str());
}

AllocaInst* CodeGenerator::GetVariable(const std::string& VarName) {
    if (!*compile) return nullptr;
    return Variables[VarName];
}

void CodeGenerator::StoreInVariable(const std::string& VarName, Value* val) {
    if (!*compile) return;
    StoreInVariable(GetVariable(VarName), val);
}


void CodeGenerator::StoreInVariable(AllocaInst* var, Value* val) {
    if (!*compile) return;
    builder->CreateStore(val, var);
}

// осторожно с bool, надо отдельно обработать
Value* CodeGenerator::CreateConst(IVariant* var) {
    if (!*compile) return nullptr;
    switch (var->Type) {
    case EValueType::IntegerValue: {
        auto tmp1 = static_cast<IntVariant*>(var);
        return ConstantInt::get(*context, APInt(32, uint64_t(tmp1->IntValue), true));
    }
    case EValueType::FloatValue: {
        auto tmp2 = static_cast<FloatVariant*>(var);
        return ConstantFP::get(Type::getDoubleTy(*context), tmp2->FloatValue);
    }
    case EValueType::CharValue: {
        auto tmp3 = static_cast<CharVariant*>(var);
        return ConstantInt::get(*context, APInt(8, uint64_t(tmp3->CharValue), true));
    }
    case EValueType::StringValue: {
        auto tmp4 = static_cast<StringVariant*>(var);
        return builder->CreateGlobalStringPtr(tmp4->StringValue);
    }
    default:
        throw "alarm, bool const created";
    }


}


Value* CodeGenerator::Plus(Value* a, Value* b, EValueType valType) {
    if (!*compile) return nullptr;
    if (valType == EValueType::IntegerValue) {
        return builder->CreateAdd(a, b, "addmpint");
    }
    else {
        return builder->CreateFAdd(a, b, "addmpfloat");
    }
}
Value* CodeGenerator::Minus(Value* a, Value* b, EValueType valType) {
    if (!*compile) return nullptr;
    if (valType == EValueType::IntegerValue) {
        return builder->CreateSub(a, b, "subtmpint");
    }
    else {
        return builder->CreateFSub(a, b, "subtmpfloat");
    }
}
Value* CodeGenerator::Mult(Value* a, Value* b, EValueType valType) {
    if (!*compile) return nullptr;
    if (valType == EValueType::IntegerValue) {
        return builder->CreateMul(a, b, "multmpint");
    }
    else {
        return builder->CreateFMul(a, b, "multmpfloat");
    }
}
Value* CodeGenerator::Divide(Value* a, Value* b, EValueType valType) {
    if (!*compile) return nullptr;
    if (valType == EValueType::IntegerValue) {
        return builder->CreateExactSDiv(a, b, "divtmpint");
    }
    else {
        return builder->CreateFDiv(a, b, "divtmpfloat");
    }
}

// вычитаем из нуля
Value* CodeGenerator::MinusConst(Value* a, EValueType valType) {
    if (!*compile) return nullptr;
    if (valType == EValueType::IntegerValue) {
        return builder->CreateSub(ConstantInt::get(*context, APInt(32, uint64_t(0), true)), a, "minusconstint");
    }
    else {
        return builder->CreateSub(ConstantFP::get(*context, APFloat(0.0)), a, "minusconstfloat");
    }
}


void CodeGenerator::createBoolConsts() {
    if (!*compile) return;
    Consts["false"] = CreateEntryBlockAlloca(builder->GetInsertBlock()->getParent(), "false", EValueType::BoolValue);
    builder->CreateStore(ConstantInt::get(*context, APInt(1, uint64_t(0), false)), Consts["false"]);
    Consts["true"] = CreateEntryBlockAlloca(builder->GetInsertBlock()->getParent(), "true", EValueType::BoolValue);
    builder->CreateStore(ConstantInt::get(*context, APInt(1, uint64_t(1), false)), Consts["true"]);
}