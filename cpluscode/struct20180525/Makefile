main:datastruct.o test.o
	g++ -o output datastruct.o test.o

datastruct.o:datastruct.cpp
	g++ -c datastruct.cpp
test.o:test.cpp test.h
	g++ -c test.cpp 

clean:
	rm -rf output *.o
