#include <time.h>
#include <iostream>
#include <string.h>
#include "minterpretator.h"

using namespace std;

int
main(int argc, char **argv)
{
	const char *select, *filter, *input, *output;
	char sep;
	
	time_t begin = time(nullptr);
	
	sep = argv[1][strlen("--sep=")];
	select = argv[2] + strlen("--select=");
	filter = argv[3] + strlen("--filter=");
	input = argv[4];
	output = argv[5];
	
	try {
		mInterpretator interp(sep, select, filter, input, output);
		
		interp.run();
	}
	catch(const char *str) {
		cout << str << endl;
	}
	
	cout << "Total time in seconds: " << (unsigned)(time(nullptr) - begin) << endl;
	
	return 0;
}
