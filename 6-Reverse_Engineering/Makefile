CC = g++
3 = ../3-Internals/
4 = ../4-User_Mode/

# XYZZ
#	X  - Reference to 6-Reverse_Engineering
#	Y  - See README.md 
#	ZZ - Example # (e.g., 01, 02, 03)

6201: 
	$(CC) -o 6201_basic_strings.exe 6201_basic_strings.cpp

6202: 
	$(CC) -o 6202_basic_strings_helper.exe 6202_basic_strings_helper.cpp

all: 
	$(MAKE) 6201
	$(MAKE) 6202

clean: 
	rm -f *.o *.exe *.so
