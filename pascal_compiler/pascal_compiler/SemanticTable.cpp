#include "SemanticTable.h"
#include "IType.h"
#include "SemanticError.h"

bool SemanticTable::NewFrame() {
	stack.push_back(StackFrame());
	return true;
}

bool SemanticTable::DeleteFrame() {
	if (stack.size() == 0) return false;
	stack.pop_back();
	return true;
}


void SemanticTable::Printf(std::vector<IType*> args) {
	if (args.size() < 1) {
		throw new SemanticError("need atleast 1 argument to call printf");
	}
	if (args[0] != GetBaseType("string")) {
		throw new SemanticError("first printf' argument needs to be string");
	}
}

Identifier* SemanticTable::find_ident(std::string name) {
	for (auto rit = stack.rbegin(); rit != stack.rend(); ++rit) {
		if (rit->Identifiers.find(name) != rit->Identifiers.end()) {
			return rit->Identifiers[name];
		}
	}
	return nullptr;
}


void SemanticTable::check_existance_on_last_frame(std::string name) {
	if (stack.back().Identifiers.find(name) != stack.back().Identifiers.end()) {
		throw new SemanticError("Identifier '" + name +  "' already exists");
	}
}

Identifier* SemanticTable::CreateIdentifier(std::string name, EIdentifierUsage usage, IType* t) {
	check_existance_on_last_frame(name);
	stack.back().Identifiers[name] = new Identifier(name, usage, t);
	return stack.back().Identifiers[name];
}

ScalarType* SemanticTable::CreateScalarType(std::string name) {
	check_existance_on_last_frame(name);
	IType* t = new ScalarType(name);
	stack.back().Types.push_back(t);
	return static_cast<ScalarType*>(t);
}

ArrayTypeSem* SemanticTable::CreateArrayType(IType* base_type, std::vector<IType*> dims) {
	/*auto base = find_ident(base_type);
	if (base == nullptr) {
		throw new SemanticError("can't find base type for array");
	}*/
	std::vector<RangeType*> tmp;
	for (auto d : dims) {
		if (d->Type != ETypes::Range) {
			throw new SemanticError("Array dimension can only be range type");
		}
		tmp.push_back(static_cast<RangeType*>(d));
	}
	if (dims.size() == 0) {
		throw new SemanticError("Can't create array with zero dims");
	}
	IType* t = new ArrayTypeSem(dims.size(), tmp, base_type);
	stack.back().Types.push_back(t);
	return static_cast<ArrayTypeSem*>(t);
}


RangeType* SemanticTable::CreateRangeType(int min, int max) {
	if (min >= max) 
		throw new SemanticError("min of range is more or equal max of range");
	
	IType* t = new RangeType(static_cast<ScalarType*>(find_ident("integer")->Type), min, max);
	stack.back().Types.push_back(t);
	return static_cast<RangeType*>(t);
}


RangeType* SemanticTable::CreateRangeType(int min, int max, IType* type) {
	if (type != GetType("integer") && type != GetType("char")) {
		throw new SemanticError("can create range only from integer-like types");
	}
	if (min >= max)
		throw new SemanticError("min of range is more or equal max of range");
	IType* t = new RangeType(static_cast<ScalarType*>(type), min, max);
	stack.back().Types.push_back(t);
	return static_cast<RangeType*>(t);
}


EnumType* SemanticTable::CreateEnumType(std::vector<std::string> idents) {
	for (auto s : idents) {
		check_existance_on_last_frame(s);
	}
	EnumType* t = new EnumType();
	for (auto s : idents) {
		t->Values.push_back(CreateIdentifier(s, EIdentifierUsage::Const, t));
	}
	return t;
}


Identifier* SemanticTable::AccessIdentifier(std::string identifier) {
	auto var = find_ident(identifier);
	if (var == nullptr) {
		auto fake = CreateIdentifier(identifier, EIdentifierUsage::Fake, nullptr);
		fake->AddFakeUsage(EIdentifierUsage::Const);
		throw new SemanticError("Can't find identifier '" + identifier + "'");
	}
	if (var->Usage == EIdentifierUsage::Procedure) {
		if (var->Name == "printf") {
			return var;
		}
		else {
			throw new SemanticError("in this version only printf procedure is available");
		}
	}
	if ((var->Usage != EIdentifierUsage::Variable) && (var->Usage != EIdentifierUsage::Const)) {
		bool ignor = false;
		if (var->CheckFakeUsage(EIdentifierUsage::Variable)) {
			ignor = true;
		}
		// будем считать, что если к идентификатору хотят обратиться, то это именованная константа 
		if (var->CheckFakeUsage(EIdentifierUsage::Const)) {
			ignor = true;
		}
		else {
			var->AddFakeUsage(EIdentifierUsage::Const);
		}
		throw new SemanticError("can access only variables and names consts", ignor);
	}
	return var;
}


IType* SemanticTable::AccessArray(std::string identifier, std::vector<IType*> indices) {
	auto arr = find_ident(identifier);
	if (arr == nullptr) {
		auto fake = CreateIdentifier(identifier, EIdentifierUsage::Fake, nullptr);
		fake->AddFakeUsage(EIdentifierUsage::Variable);
		throw new SemanticError("Can't find identifier '" + identifier);
	}
	if (arr->Usage != EIdentifierUsage::Variable) {
		if (!arr->CheckFakeUsage(EIdentifierUsage::Variable)) {
			arr->AddFakeUsage(EIdentifierUsage::Variable);
			throw new SemanticError("can't index non variable");
		}
		else {
			throw new SemanticError("can't index non variable", true);
		}
	}
	if (arr->Type->Type != ETypes::Array) {
		throw new SemanticError("Can't index non-array type");
	}
	ArrayTypeSem* arr_type = static_cast<ArrayTypeSem*>(arr->Type);
	if (arr_type->dims != indices.size()) {
		throw new SemanticError("Dimension count doesn't match");
	}
	for (int i = 0; i < arr_type->dims; i++) {
		if (arr_type->dims_list[i]->BaseType != indices[i]) {
			throw new SemanticError("Dimension " + std::to_string(i + 1) + " has wrong type, expected '" + arr_type->dims_list[i]->BaseType->Name + "'");
		}
	}

	return arr_type->BaseType;
}


IType* SemanticTable::CompareOperation(IType* a, IType* b) {
	if (a->Type != ETypes::Scalar || b->Type != ETypes::Scalar) {
		throw new SemanticError("can't compare non-scalar type");
	}
	if (a != b) {
		throw new SemanticError("can't compare non-equal types");
	}
	return stack.front().Identifiers["boolean"]->Type;
}


IType* SemanticTable::ArithmeticOperation(IType* a, IType* b) {
	if (a->Type != ETypes::Scalar || b->Type != ETypes::Scalar) {
		throw new SemanticError("can't do arithmetic with non-scalar type");
	}
	if (a->Type != b->Type) {
		// пока что, мб приведение типов сделать, но не охото...
		throw new SemanticError("can't do arithmetic with non-equal types");
	}
	return a;
}

IType* SemanticTable::GetType(std::string identifier) {
	auto ident = find_ident(identifier);
	if (ident == nullptr) {
		auto fake = CreateIdentifier(identifier, EIdentifierUsage::Fake, nullptr);
		fake->AddFakeUsage(EIdentifierUsage::Type);
		throw new SemanticError("Can't find identifier '" + identifier + "'");
	}
	if (ident->Usage != EIdentifierUsage::Type) {
		if (ident->CheckFakeUsage(EIdentifierUsage::Type)) {
			throw new SemanticError("This identifier is not type", true);
		}
		ident->AddFakeUsage(EIdentifierUsage::Type);
		throw new SemanticError("This identifier is not type");
	}
	return ident->Type;
}


IType* SemanticTable::GetBaseType(std::string identifier) {
	if (stack.front().Identifiers.find(identifier) == stack.front().Identifiers.end()) {
		throw new SemanticError("No such identifier");
	}
	auto ident = stack.front().Identifiers[identifier];
	if (ident->Usage != EIdentifierUsage::Type) {
		throw new SemanticError("This identifier is not type");
	}
	return ident->Type;
}


void SemanticTable::AssignVar(std::string identifier, IType* t) {
	auto ident = find_ident(identifier);
	if (ident == nullptr) {
		auto fake = CreateIdentifier(identifier, EIdentifierUsage::Fake, nullptr);
		fake->AddFakeUsage(EIdentifierUsage::Variable);
		throw new SemanticError("No such identifier");
	}
	if (ident->Usage != EIdentifierUsage::Variable) {
		bool ignor = ident->CheckFakeUsage(EIdentifierUsage::Variable);
		ident->AddFakeUsage(EIdentifierUsage::Variable);
		throw new SemanticError("Can't assign value to anything than variable", ignor);
	}
	if (ident->Type->Type == ETypes::Range) {
		auto range_type = static_cast<RangeType*>(ident->Type);
		if (range_type->BaseType != t) {
			throw new SemanticError("Trying to assign range var to different type");
		}

	}
	else if (ident->Type != t) {
		throw new SemanticError("Trying to assign variable to different type");
	}
}

// использовать с осторожностью
void SemanticTable::AssignVar(IType* a, IType* b) {
	if (a->Type == ETypes::Range) {
		auto range_type = static_cast<RangeType*>(a);
		if (range_type->BaseType != b) {
			throw new SemanticError("Trying to assign range var to different type");
		}

	}
	else if (a != b) {
		throw new SemanticError("Trying to assign variable to different type");
	}
}






