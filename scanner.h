#ifndef SCANNER
#define SCANNER

#include <string>
#include "lex.h"
#include "table.h"

using namespace std;

class Scanner
{
	char c;
	int i;
	const char *str;
	void gc() {c = str[i++];}
	
	enum state
	{
		H,
		COLUMN,
		INTEGER,
		DOT,
		FLOAT
	};
	
	const static char* TW[];
	const static lexType words[];
	const static char TS[];
	const static lexType seps[];
	
	lexType findSep(char) const;
	lexType findKeyword(const string&) const;
	
public:
	Scanner(const char *str_): str(str_), i(0)
	{
		if (str == 0) {
			throw "Empty input";
		}
		gc();
	}
	
	Lex gl();
	
	table <string> tID;
	table <double> tConst;
};

const char* Scanner::TW[] =
{
	"as",
	"and",
	"or",
	"not",
	"avg",
	"min",
	"max",
	nullptr
};

const lexType Scanner::words[] =
{
	LEX_AS,
	LEX_AND,
	LEX_OR,
	LEX_NOT,
	LEX_AVG,
	LEX_MIN,
	LEX_MAX,
	LEX_NULL
};

const char Scanner::TS[] =
{
	'+',
	'-',
	'*',
	'/',
	'(',
	')',
	',',
	'>',
	'<',
	'=',
	0
};

const lexType Scanner::seps[] =
{
	LEX_PLUS,
	LEX_MINUS,
	LEX_MUL,
	LEX_DIV,
	LEX_LPAREN,
	LEX_RPAREN,
	LEX_COMMA,
	LEX_GREATER,
	LEX_LESS,
	LEX_EQUAL,
	LEX_NULL
};

lexType
Scanner::findSep(char sep) const
{
	for (int i = 0; TS[i] != 0; ++i) {
		if (TS[i] == sep) {
			return seps[i];
		}
	}
	return LEX_NULL;
}

lexType
Scanner::findKeyword(const string& buf) const
{
	for (int i = 0; TW[i] != nullptr; ++i) {
		if (TW[i] == buf) {
			return words[i];
		}
	}
	return LEX_NULL;
}

Lex
Scanner::gl()
{
	state CS = H;
	string buf;
	lexType type;
	double d = 0.0;
	double p = 1.0;
	
	while (true) {		
		switch (CS) {
		case H:
			if (isalpha(c) || c == '_') {
				CS = COLUMN;
			} else if (isdigit(c)) {
				CS = INTEGER;
			} else if (c == ' ') {
				gc();
			} else if ((type = findSep(c)) != LEX_NULL) {
				gc();
				return Lex(type);
			} else if (c == '\0') {
				return Lex(LEX_FIN);
			} else {
				return Lex(LEX_ERROR);
			}
			break;
		case COLUMN:
			if (isalpha(c) || c == '_' || isdigit(c)) {
				buf.push_back(c);
				gc();
			} else {
				if ((type = findKeyword(buf)) != LEX_NULL) {
					return Lex(type);
				} else {					
					return Lex(LEX_COLUMN, tID.put(buf));
				}
			}
			break;
		case INTEGER:
			if (isdigit(c)) {
				d = d * 10.0 + (double)(c - '0');
				gc();
			} else if (c == '.') {
				CS = DOT;
				gc();
			} else {
				return Lex(LEX_NUMBER, tConst.put(d));
			}
			break;
		case DOT:
			if (!isdigit(c)) {
				return Lex(LEX_ERROR);
			} else {
				CS = FLOAT;
			}
			break;
		case FLOAT:
			if (isdigit(c)) {
				p *= 0.1;
				d += p * (double)(c - '0');
				gc();
			} else {
				return Lex(LEX_NUMBER, tConst.put(d));
			}
			break;
		default:
			return Lex(LEX_ERROR);
			break;
		}
	}
}

#endif
