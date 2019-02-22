all: csvmap csvreduce

csvmap: csvmap.o
	g++ -std=c++11 -o csvmap csvmap.o

csvmap.o: csvmap.cpp
	g++ -std=c++11 -o csvmap.o -c csvmap.cpp

csvreduce: csvreduce.o
	g++ -std=c++11 -o csvreduce csvreduce.o

csvreduce.o: csvreduce.cpp
	g++ -std=c++11 -o csvreduce.o -c csvreduce.cpp

clean:
	rm *.o
