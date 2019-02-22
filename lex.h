#ifndef LEX
#define LEX

#include <iostream>

using namespace std;

enum lexType
{
	LEX_NULL,
	LEX_COLUMN,
	LEX_NEWCOLUMN,
	LEX_NUMBER,
	LEX_TRUE,
	LEX_FALSE,
	LEX_AS,
	LEX_AND,
	LEX_ANDSEP,
	LEX_OR,
	LEX_ORSEP,
	LEX_NOT,
	LEX_AVG,
	LEX_MIN,
	LEX_MAX,
	LEX_PLUS,
	LEX_MINUS,
	LEX_MUL,
	LEX_DIV,
	LEX_UNARMINUS,
	LEX_LPAREN,
	LEX_RPAREN,
	LEX_COMMA,
	LEX_LESS,
	LEX_GREATER,
	LEX_EQUAL,
	LEX_FIN,
	LEX_ERROR
};

struct Lex
{
	lexType type;
	int value;
	Lex(lexType type_ = LEX_NULL, int value_ = 0): type(type_), value(value_) {}
	Lex(const Lex& lex): type(lex.type), value(lex.value) {}
	bool operator== (const Lex& lex) const {return type == lex.type && value == lex.value;}
	bool operator< (const Lex& lex) const {return value < lex.value;} //для сортировки
};

#endif
