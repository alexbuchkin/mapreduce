#ifndef MFPARSER
#define MFPARSER

#include <string.h>
#include "parser.h"

using namespace std;

class mfParser: public Parser
{
	void EXPR();
	void EXPR1();
	void AND();
	void AND1();
	void NOT();
	void LE();
	void AE();
	void MDM();
	void AE1();
	void MD();
	void ARG();
	void MD1();
	void ARGM();
	
public:
	mfParser(const char *str): Parser(str) {}
	void analyze()
	{
		gl();
		EXPR();
	}
};

void
mfParser::EXPR()
{
	if (lex.type == LEX_FIN) {
		prog.push_back(Lex(LEX_TRUE));
		return;
	}
	AND();
	EXPR1();
}

void
mfParser::EXPR1()
{
	if (lex.type == LEX_OR) {
		prog.push_back(Lex(LEX_ORSEP));
		gl();
		AND();
		EXPR1();
		prog.push_back(Lex(LEX_OR));
	} else if (lex.type != LEX_FIN) {
		throw "mfParser EXPR1 incorrect ending";
	}
}

void
mfParser::AND()
{
	NOT();
	AND1();
}

void
mfParser::AND1()
{
	if (lex.type == LEX_AND) {
		prog.push_back(Lex(LEX_ANDSEP));
		gl();
		NOT();
		AND1();
		prog.push_back(Lex(LEX_AND));
	}
}

void
mfParser::NOT()
{
	if (lex.type == LEX_NOT) {
		gl();
		LE();
		prog.push_back(Lex(LEX_NOT));
	} else {
		LE();
	}
}

void
mfParser::LE()
{
	AE();	
	if (lex.type == LEX_LESS || lex.type == LEX_GREATER || lex.type == LEX_EQUAL) {
		Lex l = lex;
		gl();
		AE();
		prog.push_back(l);
	} else {
		throw "mfParser LE not enough comparison signs";
	}
}

void
mfParser::AE()
{
	MDM();
	AE1();
}

void
mfParser::AE1()
{
	if (lex.type == LEX_PLUS || lex.type == LEX_MINUS) {
		Lex l = lex;
		gl();
		MD();
		AE1();
		prog.push_back(l);
	}
}

void
mfParser::MD()
{
	ARG();
	MD1();
}

void
mfParser::MD1()
{
	if (lex.type == LEX_MUL || lex.type == LEX_DIV) {
		Lex l = lex;
		gl();
		ARG();
		MD1();
		prog.push_back(l);
	}
}

void
mfParser::MDM()
{
	ARGM();
	MD1();
}

void
mfParser::ARGM()
{
	if (lex.type == LEX_MINUS) {
		gl();
		ARG();
		prog.push_back(Lex(LEX_UNARMINUS));
	} else {
		ARG();
	}
}

void
mfParser::ARG()
{
	if (lex.type == LEX_COLUMN || lex.type == LEX_NUMBER) {
		prog.push_back(lex);
		gl();
	} else if (lex.type == LEX_LPAREN) {
		gl();
		AE();
		if (lex.type != LEX_RPAREN) {
			throw "mfParser ARG, no right parenthese";
		}
		gl();
	} else {
		throw "mfParser ARG, wrong lexeme";
	}
}

#endif
