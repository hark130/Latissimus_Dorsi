CC = gcc
UP = ../

0000:
	$(CC) -o memset-0000-control.exe -I $(UP) memset-0000-control.c

1111:
	$(CC) -o memset-1111.exe -I $(UP) memset-1111.c
	
1211:
	$(CC) -o memset-1211.exe -I $(UP) memset-1211.c

1212:
	$(CC) -o memset-1212a.exe -I $(UP) memset-1212a.c

2111:
	$(CC) -o memset-2111.exe -I $(UP) memset-2111.c

2211:
	$(CC) -o memset-2211.exe -I $(UP) memset-2211.c
	
3111:
	$(CC) -o memset-3111.exe -I $(UP) memset-3111.c
	
3211:
	$(CC) -o memset-3211.exe -I $(UP) memset-3211.c

testing:
	$(CC) -o Memoroad.o -I $(UP) -c $(UP)Memoroad.c
	$(CC) -o Timeroad.o -I $(UP) -c $(UP)Timeroad.c
	$(CC) -o memset-experiment.exe -I $(UP) Memoroad.o Timeroad.o automate_memset_experiment.c

all:
	$(MAKE) 0000
	$(MAKE) 1111
	$(MAKE) 1211
	$(MAKE) 1212
	$(MAKE) 2111
	$(MAKE) 2211
	$(MAKE) 3111
	$(MAKE) 3211
	$(MAKE) testing
	
clean: 
	rm -f *.o *.exe *.so

# clear; make clean; make all; for x in `ls *.exe`; do ./$x; done