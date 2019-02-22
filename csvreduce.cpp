#include <time.h>
#include <iostream>
#include <string.h>
#include "rinterpretator.h"

using namespace std;

int
main(int argc, char **argv)
{
	time_t begin = time(nullptr);
	const char *keys, *aggrs, *input, *output;
	char sep;
	
	sep = argv[1][strlen("--sep=")];
	keys = argv[2] + strlen("--keys=");
	aggrs = argv[3] + strlen("--aggrs=");
	input = argv[4];
	output = argv[5];
	
	try {
		rInterpretator interp(sep, keys, aggrs, input, output);
		
		interp.run();
	}
	catch(const char *str) {
		cout << str << endl;
	}
	catch(...) {
		cout << "..." << endl;
	}
	
	cout << "Total time in seconds: " << (unsigned)(time(nullptr) - begin) << endl;
	
	return 0;
}
