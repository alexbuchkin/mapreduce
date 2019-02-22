#ifndef RKPARSER
#define RKPARSER

#include <vector>
#include "parser.h"

using namespace std;

class rkParser: public Parser
{
	void S();
	void S1();
	
public:
	rkParser(const char *str): Parser(str) {}
	
	table <int> keys;
	
	void analyze()
	{
		gl();
		S();
	}
};

void
rkParser::S()
{
	if (lex.type != LEX_COLUMN) {
		throw "S No column";
	}
	keys.put(lex.value);
	gl();
	S1();
}

void
rkParser::S1()
{
	if (lex.type != LEX_COMMA && lex.type != LEX_FIN) {
		throw "S1 Not a comma or a finish";
	}
	if (lex.type == LEX_COMMA) {
		gl();
		if (lex.type != LEX_COLUMN) {
			throw "S1 No column";
		}
		keys.put(lex.value);
		gl();
		S1();
	}
}

#endif
