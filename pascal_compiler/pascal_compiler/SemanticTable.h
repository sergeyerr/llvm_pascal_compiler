#pragma once
#include "IType.h"
#include <map>
#include <string>
#include <list>
#include "ValueToken.h"
class StackFrame {
public:
	std::map<std::string, Identifier*> Identifiers;
	std::list<IType*> Types;
	~StackFrame() {
		for (auto p : Identifiers) {
			delete p.second;
		}
		for (auto p : Types) {
			delete p;
		}
	}
};


class SemanticTable {
std::list<StackFrame> stack;
Identifier* find_ident(std::string);
void check_existance_on_last_frame(std::string);
public:
	bool NewFrame();
	bool DeleteFrame();
	Identifier* CreateIdentifier(std::string, EIdentifierUsage, IType*);
	ScalarType* CreateScalarType(std::string);
	RangeType* CreateRangeType(int min, int max);
	RangeType* CreateRangeType(int min, int max, IType* type);
	void Printf(std::vector<IType*> args);
	ArrayTypeSem* CreateArrayType(IType* base_type, std::vector<IType*>);
	EnumType* CreateEnumType(std::vector<std::string>);
	IType* GetType(std::string);
	IType* GetBaseType(std::string);
	IType* AccessArray(std::string, std::vector<IType*>);
	Identifier* AccessIdentifier(std::string identifier);
	IType* CompareOperation(IType*, IType*);
	IType* ArithmeticOperation(IType*, IType*);

	void AssignVar(std::string, IType*);
	// для случая с массивом
	void AssignVar(IType*, IType*);

};