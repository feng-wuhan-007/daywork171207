main:main.o test.o
	g++ -o output main.o test.o
main.o:main.cpp test.h tmp.h
	g++ -c main.cpp
test.o:test.cpp test.h
	g++ -c test.cpp

clean:
	rm -rf output *.o