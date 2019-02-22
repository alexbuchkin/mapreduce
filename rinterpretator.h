#ifndef RINTERPRETATOR
#define RINTERPRETATOR

#include <iostream>
#include <stack>
#include <algorithm>
#include <vector>
#include "table.h"
#include "lex.h"
#include "filewrapper.h"
#include "raparser.h"
#include "rkparser.h"
#include "executer.h"

using namespace std;

class rInterpretator
{
	raParser pa;//aggregation parser
	rkParser pk;//keys parser
	FileWrapper fw;//обёртка для файлов
	
	table <int> avgcolumns, mincolumns, maxcolumns;//номера столбцов-аргументов соответствующих функций
	vector <double> favg, fmin, fmax;//промежуточные значения
	int strNum;//число строк с одинаковыми значениями в ключевых столбцах
	vector <double> keyValues;//значения ключевых столбцов в данной строке
	
	bool isEqual();//проверяет, равны ли значения ключевых столбцов в текущей и предыдущей строке
	void updateKeys();//обновляет значения ключевых столбцов
	void updateFunc();//производит необходимые действия для промежуточного подсчёта функций
	void fillFunc();//заполняет значения функций в первый раз
	void a();//выполняет агрегацию
	
public:
	rInterpretator(char sep, const char* keys, const char* aggrs, const char* inName, const char* outName);	
	void run();//делает всё
};

rInterpretator::rInterpretator(char sep, const char* keys, const char* aggrs, const char* inName, const char* outName):
	pa(aggrs), pk(keys), fw(sep, inName, outName), strNum(0)
	{
		//получить полиз-массив выражения агрегации и ключевые столбцы
		//убедиться, что оба используют только существующие столбцы, заодно переписать индексы
		//удалить повторяющиеся ключевые столбцы
		//убедиться, что выходные столбцы (после as) не существуют и уникальны
		//вывести результирующие столбцы в выходной файл
		//начать подсчёт

		//получаю полиз-массивы
		pa.analyze();
		pk.analyze();
		
		//проверяю, все ли столбцы существуют, и соотношу номера в полизе и столбцы
		for (auto it = pa.prog.begin(); it != pa.prog.end(); ++it)
		{
			int i = 0;
			string buf;
			if (it->type == LEX_COLUMN) {
				buf = pa.getName(it->value);
				for (i = 0; i < fw.columns.size(); ++i) {
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
		
		for (auto it = pk.keys.vec.begin(); it != pk.keys.vec.end(); ++it) {
			int i = 0;
			string buf = pk.getName(*it);
			for (i = 0; i < fw.columns.size(); ++i) {
				if (fw.columns[i] == buf) {
					*it = i;
					break;
				}
			}
			if (i == fw.columns.size()) {
				throw string("Non-existing column \"" + buf + "\" is used").c_str();
			}
		}
		
		//убеждаюсь, что выходные столбцы (после as) не существуют и уникальны
		for (auto rit = pa.resultColumns.begin(); rit != pa.resultColumns.end(); ++rit) {
			auto cit = fw.columns.begin();
			if ((cit = find(fw.columns.begin(), fw.columns.end(), pa.getName(rit->value))) != fw.columns.end()) {
				throw string("Result column \"" + *cit + "\" is already used").c_str();
			}
		}
		
		for (auto it1 = pa.resultColumns.begin(); it1 != pa.resultColumns.end(); ++it1) {
			for (auto it2 = it1 + 1; it2 != pa.resultColumns.end(); ++it2) {
				if (it1->value == it2->value) {
					throw "Identical result columns";
				}
			}
		}
		
		//записываю в таблицу номера столбцов, являющихся аргументами функций avg, min, max
		for (auto it = pa.prog.begin(); it != pa.prog.end(); ++it) {
			if (it->type == LEX_AVG) {
				++it;
				avgcolumns.put(it->value);
			} else if (it->type == LEX_MIN) {
				++it;
				mincolumns.put(it->value);
			} else if (it->type == LEX_MAX) {
				++it;
				maxcolumns.put(it->value);
			}
		}
		
		//задаю размеры массивов, где будут храниться промежуточные результаты вычисления функций avg, min, max
		favg.resize(fw.columns.size());
		fmin.resize(fw.columns.size());
		fmax.resize(fw.columns.size());
		
		//задаю размер массива, где будут храниться значения ключевых столбцов в предыдущей строке
		keyValues.resize(pk.keys.vec.size());		
	}

bool
rInterpretator::isEqual()
{
	int i = 0;
	auto it = pk.keys.vec.begin();
	for (; it != pk.keys.vec.end(); ++it, ++i) {
		if (keyValues[i] != fw.values[*it]) {
			return false;
		}
	}
	return true;
}

void
rInterpretator::updateKeys()
{
	int i = 0;
	auto it = pk.keys.vec.begin();
	for (; it != pk.keys.vec.end(); ++it, ++i) {
		keyValues[i] = fw.values[*it];
	}
}

void
rInterpretator::updateFunc()
{
	++strNum;
	auto it = avgcolumns.vec.begin();
	for (it = avgcolumns.vec.begin(); it != avgcolumns.vec.end(); ++it) {
		favg[*it] += fw.values[*it];
	}
	for (it = mincolumns.vec.begin(); it != mincolumns.vec.end(); ++it) {
		fmin[*it] = (fw.values[*it] < fmin[*it] ? fw.values[*it] : fmin[*it]);
	}
	for (it = maxcolumns.vec.begin(); it != maxcolumns.vec.end(); ++it) {
		fmax[*it] = (fw.values[*it] > fmax[*it] ? fw.values[*it] : fmax[*it]);
	}
	
}

void
rInterpretator::fillFunc()
{
	strNum = 1;
	auto it = avgcolumns.vec.begin();
	for (it = avgcolumns.vec.begin(); it != avgcolumns.vec.end(); ++it) {
		favg[*it] = fw.values[*it];
	}
	for (it = mincolumns.vec.begin(); it != mincolumns.vec.end(); ++it) {
		fmin[*it] = fw.values[*it];
	}
	for (it = maxcolumns.vec.begin(); it != maxcolumns.vec.end(); ++it) {
		fmax[*it] = fw.values[*it];
	}
}

void
rInterpretator::a()
{
	Executer exec;
	double d;
	for (auto it = pa.prog.begin(); it != pa.prog.end(); ++it) {
		switch (it->type) {
		case LEX_AVG:
			++it;
			exec.put(favg[it->value] / (double)strNum);
			break;
		case LEX_MIN:
			++it;
			exec.put(fmin[it->value]);
			break;
		case LEX_MAX:
			++it;
			exec.put(fmax[it->value]);
			break;
		case LEX_NUMBER:
			exec.put(pa.getConstant(it->value));
			break;
		case LEX_COMMA:
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
rInterpretator::run()
{
	for (auto it = pa.resultColumns.begin(); it != pa.resultColumns.end(); ++it) {
		fw.put(pa.getName(it->value));
	}
	fw.put("\n");
	
	if (!fw.nextString()) {
		return;
	}
	
	updateKeys();
	fillFunc();
	
	while (fw.nextString()) {
		if (isEqual()) {
			updateFunc();
		} else {
			a();
			updateKeys();
			fillFunc();
			fw.put("\n");
		}
	}
	
	a();	
}

#endif
