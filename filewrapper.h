#ifndef FILEWRAPPER
#define FILEWRAPPER

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <stdio.h>

using namespace std;

class FileWrapper
{
	ifstream in;
	ofstream out;
	const char sep;
	
public:
	FileWrapper(char sep_, const char* inName, const char* outName): sep(sep_)
	{
		try {
			in.open(inName);
			if (!in.is_open()) {
				throw "Could not open input file";
			}
			out.open(outName);
			if (!out.is_open()) {
				throw "Could not open output file";
			}
		}
		catch(const string*) {
			throw;
		}
		catch(...) {
			throw "Bad files";
		}
		
		//разбиваю первую строку на названия столбцов
		string buf;
		getline(in, buf, '\n');
		buf.push_back(sep);
		size_t i = 0;
		while (i < buf.length() && buf[i] == sep) ++i;
		if (i == buf.length()) {
			throw "No column names in first string of input file";
		}
		for (size_t k = i + 1; k < buf.length(); ++k) {
			if (buf[k] == sep) {
				columns.push_back(buf.substr(i, k - i));
				i = k + 1;
				while (i < buf.length() && buf[i] == sep) ++i;
				k = i;
			}
		}
		values.resize(columns.size());
	}
	
	~FileWrapper()
	{
		in.close();
		out.close();
	}
	
	void put(const string&);
	void put(double);
	double getValue(int) const;
	bool nextString();
	
	vector <string> columns;
	vector <double> values;
};

inline double
FileWrapper::getValue(int n) const
{
	if (n < 0 || n >= values.size()) {
		throw "FW trying to access values by incorrect index";
	}
	return values[n];
}

//считать следующую строку
bool
FileWrapper::nextString()
{
	string buf;
	getline(in, buf, '\n');
	
	if (buf.empty() || buf.length() == 1 && buf.front() == '\n') {
		return false;
	}
	
	size_t i = 0;
	size_t idx = 0;
	
	for (idx = 0; idx < buf.length(); ++idx) {
		if (buf[idx] == sep) {
			buf[idx] = ' ';
		}
	}
	while (buf.back() == ' ') {
		buf.pop_back();
	}
	
	idx = 0;
	while (idx < buf.length()) {
		size_t k;
		values[i++] = stod(buf.substr(idx), &k);
		idx += k;
	}
	
	if (i != values.size()) {
		throw "Incorrect number of values in current string of the input file";
	}
	
	return true;
}

inline void
FileWrapper::put(const string& str)
{
	out << str;
	if (str != "\n") out << sep;
}

inline void
FileWrapper::put(double d)
{
	//точность
	char buf[500];
	sprintf(buf, "%.10g", d);
	out << buf << sep;
}

#endif
