all: multiThreadSorter

multiThreadSorter : multiThreadSorter.c multiThreadSorter.h
	gcc -pthread multiThreadSorter.c -o multiThreadSorter

clean :
	rm -rf multiThreadSorter
