#ifndef EXECUTER
#define EXECUTER

#include <iostream>
#include <stack>
#include "lex.h"

using namespace std;

class Executer
{
	stack <double> doubleStack;
	stack <bool> boolStack;
public:
	void put(lexType);
	void put(double);
	void put(bool);
	bool getBool();
	double getDouble();
	
	int sizeDouble() const {return doubleStack.size();}
};

inline void
Executer::put(double d)
{
	doubleStack.push(d);
}

inline void
Executer::put(bool b)
{
	boolStack.push(b);
}

void
Executer::put(lexType type)
{
	double d1, d2;
	bool b1, b2;
	
	switch (type) {
	case LEX_AND:
		b2 = boolStack.top();
		boolStack.pop();
		b1 = boolStack.top();
		boolStack.pop();
		boolStack.push(b1 && b2);
		break;
	case LEX_OR:
		b2 = boolStack.top();
		boolStack.pop();
		b1 = boolStack.top();
		boolStack.pop();
		boolStack.push(b1 || b2);
		break;
	case LEX_NOT:
		b1 = boolStack.top();
		boolStack.pop();
		boolStack.push(!b1);
		break;
	case LEX_DIV:
		d2 = doubleStack.top();
		doubleStack.pop();
		d1 = doubleStack.top();
		doubleStack.pop();
		doubleStack.push(d1 / d2);
		break;
	case LEX_MUL:
		d2 = doubleStack.top();
		doubleStack.pop();
		d1 = doubleStack.top();
		doubleStack.pop();
		doubleStack.push(d1 * d2);
		break;
	case LEX_PLUS:
		d2 = doubleStack.top();
		doubleStack.pop();
		d1 = doubleStack.top();
		doubleStack.pop();
		doubleStack.push(d1 + d2);
		break;
	case LEX_MINUS:
		d2 = doubleStack.top();
		doubleStack.pop();
		d1 = doubleStack.top();
		doubleStack.pop();
		doubleStack.push(d1 - d2);
		break;
	case LEX_UNARMINUS:
		d1 = doubleStack.top();
		doubleStack.pop();
		doubleStack.push(-d1);
		break;
	case LEX_EQUAL:
		d2 = doubleStack.top();
		doubleStack.pop();
		d1 = doubleStack.top();
		doubleStack.pop();
		boolStack.push(d1 == d2);
		break;
	case LEX_GREATER:
		d2 = doubleStack.top();
		doubleStack.pop();
		d1 = doubleStack.top();
		doubleStack.pop();
		boolStack.push(d1 > d2);
		break;
	case LEX_LESS:
		d2 = doubleStack.top();
		doubleStack.pop();
		d1 = doubleStack.top();
		doubleStack.pop();
		boolStack.push(d1 < d2);
		break;
	case LEX_TRUE:
		boolStack.push(true);
		break;
	case LEX_FALSE:
		boolStack.push(false);
		break;
	default:
		throw "Incorrect lexeme";
		break;
	}
}

inline double
Executer::getDouble()
{
	double d = doubleStack.top();
	doubleStack.pop();
	return d;
}

inline bool
Executer::getBool()
{
	bool b = boolStack.top();
	boolStack.pop();
	return b;
}

#endif
