CC = gcc
3 = ../
4 = ../../4-User_Mode/

0000:
	$(CC) -o memset-0000-control.exe -I $(3) memset-0000-control.c

1111:
	$(CC) -o memset-1111.exe -I $(3) memset-1111.c
	
1211:
	$(CC) -o memset-1211.exe -I $(3) memset-1211.c

1212:
	$(CC) -o memset-1212a.exe -I $(3) memset-1212a.c

2111:
	$(CC) -o memset-2111.exe -I $(3) memset-2111.c

2211:
	$(CC) -o memset-2211.exe -I $(3) memset-2211.c
	
3111:
	$(CC) -o memset-3111.exe -I $(3) memset-3111.c
	
3211:
	$(CC) -o memset-3211.exe -I $(3) memset-3211.c

testing:
	$(CC) -o Fileroad.o -I $(3) -c $(3)Fileroad.c
	$(CC) -o Map_Memory.o -I $(3) -I $(4) -c $(4)Map_Memory.c
	$(CC) -o Memoroad.o -I $(3) -c $(3)Memoroad.c
	$(CC) -o Timeroad.o -I $(3) -c $(3)Timeroad.c
	$(CC) -o memset-experiment.exe -I $(3) -I $(4) Fileroad.o Map_Memory.o Memoroad.o Timeroad.o automate_memset_experiment.c

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