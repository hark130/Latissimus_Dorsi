CC = gcc
3 = ./3-Internals/

print_PID_libraries:
	$(CC) -o $(3)Harkledir.o -c $(3)Harkledir.c
	$(CC) -o $(3)Harkleproc.o -c $(3)Harkleproc.c
	$(CC) -o $(3)Memoroad.o -c $(3)Memoroad.c
	$(CC) -o $(3)Fileroad.o -c $(3)Fileroad.c
	$(CC) -o $(3)3-10_Print_PID_Libraries-2_main.o -c $(3)3-10_Print_PID_Libraries-2_main.c
	$(CC) -o print_PID_libraries.exe $(3)Harkledir.o $(3)Harkleproc.o $(3)Memoroad.o $(3)Fileroad.o $(3)3-10_Print_PID_Libraries-2_main.o

all:
	$(MAKE) print_PID_libraries

clean: 
	rm -f *.o *.exe *.so
