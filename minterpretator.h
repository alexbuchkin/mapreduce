#ifndef MINTERPRETATOR
#define MINTERPRETATOR

#include <iostream>
#include <stack>
#include <algorithm>
#include "filewrapper.h"
#include "msparser.h"
#include "mfparser.h"
#include "executer.h"

using namespace std;

class mInterpretator
{
	msParser ps;//select parser
	mfParser pf;//filter parser
	FileWrapper fw;//обёртка для файлов
	
	bool f();//вычисляет выражение фильтрации
	void s();//вычисляет выражение селекции
public:
	mInterpretator(char sep, const char* select, const char* filter, const char* inName, const char* outName);
	void run();//делает всё
};

mInterpretator::mInterpretator(char sep, const char* select, const char* filter, const char* inName, const char* outName):
ps(select), pf(filter), fw(sep, inName, outName)
{
	//получить оба полиз-массива
	//убедиться, что оба используют только существующие столбцы, заодно переписать индексы
	//убедиться, что выходные столбцы (после as) не существуют и уникальны
	//вывести результирующие столбцы в выходной файл
	//начать подсчёт
	
	//получаю полиз-массивы
	ps.analyze();
	pf.analyze();
	
	//проверяю, все ли столбцы существуют, и соотношу номера в полизе и столбцы
	for (auto it = ps.prog.begin(); it != ps.prog.end(); ++it)
	{
		int i = 0;
		string buf;
		if (it->type == LEX_COLUMN) {
			buf = ps.getName(it->value);
			for (int i = 0; i < fw.columns.size(); ++i) {
				if (fw.columns[i] == buf) {
					it->value = i;
					break;
				}
			}
			if (i == fw.columns.size()) {
				throw string("Non-existing column \"" + buf + "\" is used").c_str();
			}
		}
	}
	
	for (auto it = pf.prog.begin(); it != pf.prog.end(); ++it)
	{
		int i = 0;
		string buf;
		if (it->type == LEX_COLUMN) {
			buf = pf.getName(it->value);
			for (int i = 0; i < fw.columns.size(); ++i) {
				if (fw.columns[i] == buf) {
					it->value = i;
					break;
				}
			}
			if (i == fw.columns.size()) {
				throw string("Non-existing column \"" + buf + "\" is used").c_str();
			}
		}
	}
	
	//убеждаюсь, что выходные столбцы (после as) не существуют и уникальны
	for (auto rit = ps.resultColumns.begin(); rit != ps.resultColumns.end(); ++rit) {
		if (rit->type == LEX_NEWCOLUMN) {
			auto cit = fw.columns.begin();
			if ((cit = find(fw.columns.begin(), fw.columns.end(), ps.getName(rit->value))) != fw.columns.end()) {
				throw string("Result column \"" + *cit + "\" is already used").c_str();
			}
		}
	}
	
	for (auto it1 = ps.resultColumns.begin(); it1 != ps.resultColumns.end(); ++it1) {
		for (auto it2 = it1 + 1; it2 != ps.resultColumns.end(); ++it2) {
			if (it1->value == it2->value) {
				throw "Identical result columns";
			}
		}
	}
}

bool
mInterpretator::f()
{
	Executer exec;
	bool b;
	for (auto it = pf.prog.begin(); it != pf.prog.end(); ++it) {
		switch (it->type) {
		case LEX_NUMBER:
			exec.put(pf.getConstant(it->value));
			break;
		case LEX_COLUMN:
			exec.put(fw.values[it->value]);
			break;
		case LEX_ANDSEP://сокращённое вычисление для AND
			b = exec.getBool();
			if (b == false) {
				while (it->type != LEX_AND) {
					++it;
				}
			}
			exec.put(b);
			break;
		case LEX_ORSEP://сокращённое вычисление для OR
			b = exec.getBool();
			if (b == true) {
				while (it->type != LEX_OR) {
					++it;
				}
			}
			exec.put(b);
			break;
		default:
			exec.put(it->type);
			break;
		}
	}
	
	return exec.getBool();
}

void
mInterpretator::s()
{
	Executer exec;
	double d;
	for (auto it = ps.prog.begin(); it != ps.prog.end(); ++it) {
		switch (it->type) {
		case LEX_NUMBER:
			exec.put(ps.getConstant(it->value));
			break;
		case LEX_COLUMN:
			exec.put(fw.values[it->value]);
			break;
		case LEX_COMMA://вывод значения в файл
			d = exec.getDouble();
			fw.put(d);
			break;
		default:
			exec.put(it->type);
			break;
		}
	}
}

void
mInterpretator::run()
{
	//вывожу результирующие столбцы в выходной файл
	for (auto it = ps.resultColumns.begin(); it != ps.resultColumns.end(); ++it) {
		fw.put(ps.getName(it->value));
	}
	fw.put("\n");
		
	while (fw.nextString()) {
		if (f()) {
			s();
			fw.put("\n");
		}
	}
}

#endif
