#pragma once
#include "IToken.h"
#include "IdentifierToken.h"
#include "ValueToken.h"
#include "OperationToken.h"
#include "LexicalAnalyzer.h"
#include <iomanip>

std::ostream& operator<<(std::ostream& os, const IdentifierToken& token)
{
    os << std::setw(20) << std::left;
    os << "Identifier: " << token.Name;
    return os;
}

std::ostream& operator<<(std::ostream& os, const ValueToken& token)
{
    std::string type;
    os << std::setw(20) << std::left;
    os << "Value: ";
    switch (token.Variant->Type) {
    case EValueType::CharValue:
        os << "Char";
        break;
    case EValueType::FloatValue:
        os  << "Float";
        break;
    case EValueType::StringValue:
        os << "String";
        break;
    case EValueType::IntegerValue:
        os << "Int";
        break;
    }
    return os;
}


std::ostream& operator<<(std::ostream& os, const OperationToken& token)
{
    // не хотелось заводить ещё одну статическую переменную под обратный словарь
    os << std::setw(20) << std::left;
    for (auto key_val : LexicalAnalyzer::keywordDict) {
        auto key = key_val.first;
        auto val = key_val.second;
        if (val == token.Operation) {
            os << "Operation: " << key;
            break;
        }
    }
    return os;
}