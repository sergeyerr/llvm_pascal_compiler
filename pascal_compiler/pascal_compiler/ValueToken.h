#pragma once
#include "ETokenType.h"
#include "EValueType.h"
#include "IToken.h"
#include "StringVariant.h"
#include "IntVariant.h"
#include "CharVariant.h"
#include "FloatVariant.h"
#include "BoolVariant.h"

class ValueToken : public IToken {
public: 
	IVariant* Variant;
	ValueToken(std::string str) : IToken(ETokenType::Value), Variant(new StringVariant(str)) {};
	ValueToken(int i) : IToken(ETokenType::Value), Variant(new IntVariant(i)) {};
	ValueToken(char ch) : IToken(ETokenType::Value), Variant( new CharVariant(ch)) {};
	ValueToken(float f) : IToken(ETokenType::Value), Variant(new FloatVariant(f)) {};
	ValueToken(bool b) : IToken(ETokenType::Value), Variant(new BoolVariant(b)) {};
	~ValueToken() {
		delete Variant;
	}
};