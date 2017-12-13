main:hello.o test.o
	gcc -o man hello.o test.o
hello.o:
	gcc -c hello.c
test.o:
	gcc -c test.c
clean:
	rm -rf *.o man
