#ifndef MSPARSER
#define MSPARSER

#include <vector>
#include "parser.h"

using namespace std;

class msParser: public Parser
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
	
	bool oneColumn;//если команда состоит из одного названия столбца, например "group,"
	int cValue;
	
public:
	msParser(const char *str): Parser(str) {}
	void analyze()
	{
		gl();
		S();
	}
	
	vector <Lex> resultColumns;
};

void
msParser::S()
{
	oneColumn = true;
	EXPR();
	S1();
}

void
msParser::S1()
{
	if (oneColumn) {
		resultColumns.push_back(Lex(LEX_COLUMN, cValue));
	}
	if (lex.type == LEX_COMMA) {
		prog.push_back(lex);
		gl();
		oneColumn = true;
		EXPR();
		S1();
	} else if (lex.type == LEX_FIN) {
		prog.push_back(Lex(LEX_COMMA));
	} else {
		throw "S1, Incorrect ending";
	}
}

void
msParser::EXPR()
{
	AE();
	if (lex.type == LEX_AS) {
		gl();
		if (lex.type != LEX_COLUMN) {
			throw "EXPR, no column after \"as\"";
		}
		resultColumns.push_back(Lex(LEX_NEWCOLUMN, lex.value));
		gl();
	} else {
		if (!oneColumn) {
			throw "EXPR, Not a single column with no \"as\"";
		}
		//usingColumns.put(lex);
	}
}

void
msParser::AE()
{
	MDM();
	AE1();
}

void
msParser::AE1()
{
	if (lex.type == LEX_PLUS || lex.type == LEX_MINUS) {
		Lex l = lex;
		gl();
		MD();
		prog.push_back(l);
		oneColumn = false;
		AE1();
	}
}

void
msParser::MD()
{
	ARG();
	MD1();
}

void
msParser::MD1()
{
	if (lex.type == LEX_MUL || lex.type == LEX_DIV) {
		Lex l = lex;
		gl();
		ARG();
		prog.push_back(l);
		oneColumn = false;
		MD1();
	}
}

void
msParser::MDM()
{
	ARGM();
	MD1();
}

void
msParser::ARGM()
{
	if (lex.type == LEX_MINUS) {
		gl();
		ARG();
		oneColumn = false;
		prog.push_back(Lex(LEX_UNARMINUS));
	} else {
		ARG();
	}
}

void
msParser::ARG()
{	
	if (lex.type == LEX_COLUMN) {
		prog.push_back(lex);
		cValue = lex.value;
		gl();
	} else if (lex.type == LEX_NUMBER) {
		prog.push_back(lex);
		oneColumn = false;
		gl();
	} else if (lex.type == LEX_LPAREN) {
		gl();
		AE();
		if (lex.type != LEX_RPAREN) {
			throw "ARG, no right parenthesis";
		}
		gl();
	} else {
		throw "ARG wrong lexeme";
	}
}

#endif
