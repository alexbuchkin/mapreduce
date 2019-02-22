#ifndef RAPARSER
#define RAPARSER

#include <vector>
#include "parser.h"

using namespace std;

class raParser: public Parser
{
	void S();
	void S1();
	void EXPR();
	void AE();
	void AE1();
	void MDM();
	void MD();
	void MD1();
	void ARG();
	void ARGM();
	
public:
	raParser(const char *str): Parser(str) {}
	void analyze()
	{	
		gl();
		S();
	}
	
	vector <Lex> resultColumns;
};

void
raParser::S()
{
	EXPR();
	S1();
}

void
raParser::S1()
{
	if (lex.type == LEX_COMMA) {
		prog.push_back(lex);
		gl();
		EXPR();
		S1();
	} else if (lex.type == LEX_FIN) {
		prog.push_back(Lex(LEX_COMMA));
	} else {
		throw "S1, Incorrect ending";
	}
}

void
raParser::EXPR()
{
	AE();
	if (lex.type != LEX_AS) {
		throw "EXPR, no \"as\"";
	}
	gl();
	if (lex.type != LEX_COLUMN) {
		throw "EXPR, no column after \"as\"";
	}
	resultColumns.push_back(Lex(LEX_NEWCOLUMN, lex.value));
	gl();
}

void
raParser::AE()
{
	MDM();
	AE1();
}

void
raParser::AE1()
{
	if (lex.type == LEX_PLUS || lex.type == LEX_MINUS) {
		Lex l = lex;
		gl();
		MD();
		prog.push_back(l);
		AE1();
	}
}

void
raParser::MD()
{
	ARG();
	MD1();
}

void
raParser::MD1()
{
	if (lex.type == LEX_MUL || lex.type == LEX_DIV) {
		Lex l = lex;
		gl();
		ARG();
		prog.push_back(l);
		MD1();
	}
}

void
raParser::MDM()
{
	ARGM();
	MD1();
}

void
raParser::ARGM()
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
raParser::ARG()
{	
	if (lex.type == LEX_NUMBER) {
		prog.push_back(lex);
		gl();
	} else if (lex.type == LEX_AVG) {
		gl();
		if (lex.type != LEX_LPAREN) {
			throw "ARG no left parenthesis after avg";
		}
		gl();
		if (lex.type != LEX_COLUMN) {
			throw "ARG incorrect argument of avg";
		}
		Lex l = lex;
		gl();
		if (lex.type != LEX_RPAREN) {
			throw "ARG no right parenthesis after avg";
		}
		gl();
		prog.push_back(Lex(LEX_AVG));
		prog.push_back(l);
	} else if (lex.type == LEX_MIN) {
		gl();
		if (lex.type != LEX_LPAREN) {
			throw "ARG no left parenthesis after min";
		}
		gl();
		if (lex.type != LEX_COLUMN) {
			throw "ARG incorrect argument of min";
		}
		Lex l = lex;
		gl();
		if (lex.type != LEX_RPAREN) {
			throw "ARG no right parenthesis after min";
		}
		gl();
		prog.push_back(Lex(LEX_MIN));
		prog.push_back(l);
	} else if (lex.type == LEX_MAX) {
		gl();
		if (lex.type != LEX_LPAREN) {
			throw "ARG no left parenthesis after max";
		}
		gl();
		if (lex.type != LEX_COLUMN) {
			throw "ARG incorrect argument of max";
		}
		Lex l = lex;
		gl();
		if (lex.type != LEX_RPAREN) {
			throw "ARG no right parenthesis after max";
		}
		gl();
		prog.push_back(Lex(LEX_MAX));
		prog.push_back(l);
	} else if (lex.type == LEX_COLUMN) {
		//функция
		Lex f = lex;//название функции
		gl();
		if (lex.type != LEX_LPAREN) {
			throw string("ARG No left parenthesis after function " + getName(f.value)).c_str();
		}
		gl();
		if (lex.type != LEX_COLUMN) {
			throw string("ARG Incorrect argument of function " + getName(f.value)).c_str();
		}
		Lex arg = lex;
		gl();
		if (lex.type != LEX_RPAREN) {
			throw string("ARG No right parenthesis after function " + getName(f.value)).c_str();
		}
		gl();
		prog.push_back(f);
		prog.push_back(arg);
	} else if (lex.type == LEX_LPAREN) {
		gl();
		AE();
		if (lex.type != LEX_RPAREN) {
			throw "ARG Incorrect parenthesis";
		}
		gl();
	} else {
		throw "ARG Wrong lexeme";
	}
}

#endif
