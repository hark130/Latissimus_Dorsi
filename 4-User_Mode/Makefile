CC = gcc

421: 
	$(CC) -fpic -shared -o lib421PICpractice.so 4-2_PIC_Practice-1_so.c
	$(CC) -L. -o 4-2_PIC_Practice-1.exe 4-2_PIC_Practice-1_main.c -l421PICpractice -ldl

ALL: 
	421

clean: 
	rm -f *.o *.exe *.so
