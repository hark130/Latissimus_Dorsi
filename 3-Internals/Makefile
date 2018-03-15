CC = gcc

# 331: 
# 	$(CC) -c Fileroad_Descriptors.c
# 	$(CC) -o 3-3_File_Descriptor_Practice-1_main.exe Fileroad_Descriptors.o 3-3_File_Descriptor_Practice-1_main.c

341:
	$(CC) -c Memoroad.c
	$(CC) -c Signaleroad.c
	$(CC) -c 3-04_Signal_Handling-1_nosig.c
	$(CC) -o nosig.exe Memoroad.o Signaleroad.o 3-04_Signal_Handling-1_nosig.o
	# Comment these proof of concepts source files out later
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

tests:
	$(CC) -c Memoroad.c
	$(CC) -c Fileroad.c
	$(CC) -c 3-10_Fileroad_Tests-2_main.c
	$(CC) -o 3-10_Fileroad_Tests-2_main.exe Memoroad.o Fileroad.o 3-10_Fileroad_Tests-2_main.o

hello:
	$(CC) -o HelloWorld.exe 3-10_Proc_Walk-2_Hello_World.c

waiting:
	$(CC) -o Waiting.exe 3-04_Signal_Handling-1_Waiting.c

all:
	# $(MAKE) 331
	$(MAKE) 341
	$(MAKE) 3101
	$(MAKE) 3102

clean: 
	rm -f *.o *.exe *.so

### OLD 3102 ###
# $(CC) -o ./Map_Memory.o -c ../4-User_Mode/Map_Memory.c
# $(CC) -c -I ../4-User_Mode/ Harkleproc.c
# $(CC) -o 3-10_Proc_Walk-2_main.exe Harkledir.o Harkleproc.o ./Map_Memory.o 3-10_Proc_Walk-2_main.c
################