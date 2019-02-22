#ifndef PARSER
#define PARSER

#include "scanner.h"
#include <vector>

using namespace std;

class Parser
{
protected:
	Lex lex;
	void gl() {lex = sc.gl();}
	Scanner sc;
public:
	Parser(const char *str): sc(str) {}
	vector <Lex> prog;
	
	double getConstant(int n) const {return sc.tConst[n];}
	string getName(int n) const {return sc.tID[n];}
	
	void analyze();
};

#endif
