CC = gcc

331:
	$(CC) -c Fileroad.c
	$(CC) -c Fileroad_Descriptors.c
	$(CC) -c Memoroad.c
	$(CC) -o 3-03_File_Descriptor_Practice-1_main.exe Fileroad.o Fileroad_Descriptors.o Memoroad.o 3-03_File_Descriptor_Practice-1_main.c

332:
	$(CC) -c Fileroad.c
	$(CC) -c Fileroad_Descriptors.c
	$(CC) -c Memoroad.c
	$(CC) -c Timeroad.c
	$(CC) -c 3-03_File_Descriptor_Practice-2_redirect_bin_output.c
	$(CC) -o redirect_bin_output.exe Fileroad.o Fileroad_Descriptors.o Memoroad.o Timeroad.o 3-03_File_Descriptor_Practice-2_redirect_bin_output.o

341:
	$(CC) -c Memoroad.c
	$(CC) -c Signaleroad.c
	$(CC) -c 3-04_Signal_Handling-1_nosig.c
	$(CC) -o nosig.exe Memoroad.o Signaleroad.o 3-04_Signal_Handling-1_nosig.o
	$(CC) -o 3-04_Signal_Handling-1_PoC.exe 3-04_Signal_Handling-1_PoC.c
	$(CC) -c 3-04_Signal_Handling-1_PoC2.c
	$(CC) -o 3-04_Signal_Handling-1_PoC2.exe Memoroad.o Signaleroad.o 3-04_Signal_Handling-1_PoC2.o

3101:
	$(CC) -c Harkledir.c
	$(CC) -c Harkleproc.c
	$(CC) -c Memoroad.c
	$(CC) -c Fileroad.c
	$(CC) -c 3-10_Module_Inspection-1_main.c
	$(CC) -o 3-10_Module_Inspection-1_main.exe Harkledir.o Harkleproc.o Memoroad.o Fileroad.o 3-10_Module_Inspection-1_main.o

3102:
	$(CC) -c Harkledir.c
	$(CC) -c Harkleproc.c
	$(CC) -c Memoroad.c
	$(CC) -c Fileroad.c
	$(CC) -c 3-10_Proc_Walk-2_main.c
	$(CC) -o 3-10_Proc_Walk-2_main.exe Harkledir.o Harkleproc.o Memoroad.o Fileroad.o 3-10_Proc_Walk-2_main.o
	$(CC) -c 3-10_Print_PID_Libraries-2_main.c
	$(CC) -o print_PID_libraries.exe Harkledir.o Harkleproc.o Memoroad.o Fileroad.o 3-10_Print_PID_Libraries-2_main.o
	
3181:
	$(CC) -c Fileroad.c
	$(CC) -c Fileroad_Descriptors.c
	$(CC) -c Harklecurse.c
	$(CC) -c Harklemath.c
	$(CC) -c Harklepipe.c
	$(CC) -c -pthread Harklethread.c
	$(CC) -c Memoroad.c
	$(CC) -c Rando.c
	$(CC) -c Thread_Racer.c
	$(CC) -c 3-18_Threading_Practice-1_Grand_Prix.c
	$(CC) -o grand_prix.exe -pthread Fileroad.o Fileroad_Descriptors.o Harklecurse.o Harklemath.o Harklepipe.o Harklethread.o Memoroad.o Rando.o Thread_Racer.o 3-18_Threading_Practice-1_Grand_Prix.o -lncurses -lm

3221:
	$(CC) -c Harkledir.c
	$(CC) -c Harkleproc.c
	$(CC) -c Harkletrace.c
	$(CC) -c Memoroad.c
	$(CC) -c Fileroad.c
	$(CC) -o pmparser.o -c $(PMP)pmparser.c
	$(CC) -o Map_Memory.o -c $(4UM)Map_Memory.c
	nasm -f elf64 3-22-1_Payloads/payload_64_write_1.nasm
	nasm -f elf64 3-22-1_Payloads/payload_64_write_2.nasm
	$(CC) -I $(PMP) -I $(4UM) -c 3-22_Process_Injection-1_injector.c
	$(CC) -o injector.exe Harkledir.o Harkleproc.o Harkletrace.o Memoroad.o Fileroad.o pmparser.o Map_Memory.o 3-22_Process_Injection-1_injector.o

tests:
	$(CC) -c Fileroad.c
	$(CC) -c Harklemath.c
	$(CC) -c Memoroad.c
	# $(CC) -c 3-10_Fileroad_Tests-2_main.c
	$(CC) -c 3-18_Harklemath_Tests-1_main.c
	# $(CC) -o 3-10_Fileroad_Tests-2_main.exe Memoroad.o Fileroad.o 3-10_Fileroad_Tests-2_main.o
	$(CC) -o 3-18_Harklemath_Tests-1_main.exe Harklemath.o 3-18_Harklemath_Tests-1_main.o -lm

echo:
	$(CC) -o echo_this.exe 3-04_Signal_Handling-1_echo_this.c

hello:
	$(CC) -o HelloWorld.exe 3-10_Proc_Walk-2_Hello_World.c

waiting:
	$(CC) -o Waiting.exe 3-04_Signal_Handling-1_Waiting.c

randowave:
	$(CC) -c Rando.c
	$(CC) -c 3-22_Process_Injection-1_Randowave.c
	$(CC) -o Randowave.exe Rando.o 3-22_Process_Injection-1_Randowave.o

all:
	$(MAKE) 331
	$(MAKE) 332
	$(MAKE) 341
	$(MAKE) 3101
	$(MAKE) 3102
	$(MAKE) 3181
	$(MAKE) 3221

clean: 
	rm -f *.o *.exe *.so
