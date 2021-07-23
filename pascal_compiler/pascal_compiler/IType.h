#pragma once
#include <vector>
#include <string>
#include <set>
enum class EIdentifierUsage
{
	Undefined,
	Const,
	Type,
	Variable,
	Procedure,
	Fake
};

enum class ETypes {
	Array,
	Enum,
	Range,
	Scalar,
};

class IType {
public:
	const ETypes Type;
	IType(ETypes t) : Type(t) {};
};

struct Identifier {
private:
	std::set<EIdentifierUsage> FakeUsages;
	//std::set<ETypes> FakeTypes;
public:
	const std::string Name;
	EIdentifierUsage Usage;
	IType* Type;
	
	Identifier(std::string n) : Name(n), Usage(EIdentifierUsage::Undefined), Type(nullptr) {};
	Identifier(std::string n, EIdentifierUsage usage) : Name(n), Usage(usage), Type(nullptr) {};
	Identifier(std::string n, EIdentifierUsage usage, IType* t) : Name(n), Usage(usage), Type(t) {};

	void AddFakeUsage(EIdentifierUsage u) {
		FakeUsages.insert(u);
	}

	//void AddFakeType(ETypes t) {
	//	FakeTypes.insert(t);
	//}
	//bool CheckFakeType(ETypes t) {
	//	return FakeTypes.find(t) != FakeTypes.end();
	//}

	bool CheckFakeUsage(EIdentifierUsage u) {
		return FakeUsages.find(u) != FakeUsages.end();
	}


};




class ScalarType : public IType {
public:
	const std::string Name;
	ScalarType(std::string n) : Name(n), IType(ETypes::Scalar) {};
};

class RangeType : public IType {
public:
	ScalarType* BaseType;
	// только int и char 
	const int min, max;
	RangeType(ScalarType* t, int min_, int max_) : BaseType(t), min(min_), max(max_), IType(ETypes::Range) {};


};

class EnumType : public IType {
public:
	std::vector<Identifier*> Values;
	EnumType(std::vector<Identifier*> vals) : Values(vals), IType(ETypes::Enum) {};
	EnumType() : IType(ETypes::Enum) {};
};

class ArrayTypeSem : public IType {
public:
	const int dims;
	std::vector<RangeType*> dims_list;
	IType* BaseType;
	ArrayTypeSem(int dim_, std::vector<RangeType*> dims_list_, IType* base) : dims(dim_), dims_list(dims_list_), BaseType(base), IType(ETypes::Array) {};

};

