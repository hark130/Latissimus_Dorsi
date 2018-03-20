CC = gcc
3 = ./3-Internals/

nosig:
	$(CC) -o $(3)Memoroad.o -c $(3)Memoroad.c
	$(CC) -o $(3)Signaleroad.o -c $(3)Signaleroad.c
	$(CC) -o $(3)3-04_Signal_Handling-1_nosig.o -c $(3)3-04_Signal_Handling-1_nosig.c
	$(CC) -o nosig.exe $(3)Memoroad.o $(3)Signaleroad.o $(3)3-04_Signal_Handling-1_nosig.o

print_PID_libraries:
	$(CC) -o $(3)Fileroad.o -c $(3)Fileroad.c
	$(CC) -o $(3)Harkledir.o -c $(3)Harkledir.c
	$(CC) -o $(3)Harkleproc.o -c $(3)Harkleproc.c
	$(CC) -o $(3)Memoroad.o -c $(3)Memoroad.c
	$(CC) -o $(3)3-10_Print_PID_Libraries-2_main.o -c $(3)3-10_Print_PID_Libraries-2_main.c
	$(CC) -o print_PID_libraries.exe $(3)Fileroad.o $(3)Harkledir.o $(3)Harkleproc.o $(3)Memoroad.o $(3)3-10_Print_PID_Libraries-2_main.o

redirect_bin_output:
	$(CC) -o $(3)Fileroad.o -c $(3)Fileroad.c
	$(CC) -o $(3)Fileroad_Descriptors.o -c $(3)Fileroad_Descriptors.c
	$(CC) -o $(3)Memoroad.o -c $(3)Memoroad.c
	$(CC) -o $(3)Timeroad.o -c $(3)Timeroad.c
	$(CC) -o $(3)3-03_File_Descriptor_Practice-2_redirect_bin_output.o -c $(3)3-03_File_Descriptor_Practice-2_redirect_bin_output.c
	$(CC) -o redirect_bin_output.exe $(3)Fileroad.o $(3)Fileroad_Descriptors.o $(3)Memoroad.o $(3)Timeroad.o $(3)3-03_File_Descriptor_Practice-2_redirect_bin_output.o

all:
	$(MAKE) nosig
	$(MAKE) print_PID_libraries
	$(MAKE) redirect_bin_output

clean: 
	rm -f *.o *.exe *.so
