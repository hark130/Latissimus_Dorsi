CC = gcc

421: 
	$(CC) -fpic -shared -o lib421PICpractice.so 4-2_PIC_Practice-1_so.c
	$(CC) -L. -o 4-2_PIC_Practice-1.exe 4-2_PIC_Practice-1_main.c -l421PICpractice -ldl

451:
	$(CC) -o 4-5_DCE_Practice-1_HelloWorld.exe 4-5_DCE_Practice-1_HelloWorld.c
	$(CC) -fpic -shared -o lib451DCE_evil_puts.so 4-5_DCE_Practice-1_EvilPuts.c
	# $(CC) -fpic -shared -o lib451DCE_evil_stdio.so 4-5_DCE_Practice-1_EvilStdio.c
	# $(CC) -L. -o 4-5_DCE_Practice-1_EvilDeeds.exe 4-5_DCE_Practice-1_evilMain.c -l451DCE_evil_stdio -ldl

ALL: 
	421
	451

clean: 
	rm -f *.o *.exe *.so
