CC = gcc
3 = ../3-Internals/

421: 
	$(CC) -fpic -shared -o lib421PICpractice.so 4-2_PIC_Practice-1_so.c
	$(CC) -L. -o 4-2_PIC_Practice-1.exe 4-2_PIC_Practice-1_main.c -l421PICpractice -ldl

451:
	$(CC) -o 4-5_DCE_Practice-1_HelloWorld.exe 4-5_DCE_Practice-1_HelloWorld.c
	$(CC) -fpic -shared -o lib451DCE_evil_puts.so 4-5_DCE_Practice-1_EvilPuts.c

452:
	$(CC) -c Elf_Manipulation.c
	$(CC) -I $(3) -c Map_Memory.c
	$(CC) -o HelloWorld.exe 4-5_DCE_Practice-1_HelloWorld.c
	$(CC) -o 4-5_DCE_Practice-2_ModEntryPoint.exe Elf_Manipulation.o Map_Memory.o 4-5_DCE_Practice-2_ModEntryPoint.c
	### UNIT TESTS ###
	# $(CC) -o 4-5_DCE_Practice-2_MapMemTest.exe Map_Memory.o 4-5_DCE_Practice-2_MapMemTest.c
	# $(CC) -o 4-5_DCE_Practice-2_FindCaveTest.exe Elf_Manipulation.o Map_Memory.o 4-5_DCE_Practice-2_FindCaveTest.c
	### DO NOT DELETE ###

all: 
	421
	451
	452

clean: 
	rm -f *.o *.exe *.so
