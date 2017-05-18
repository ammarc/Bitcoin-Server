#  Project 2 for COMP30023: Computer Systems
#  at the University of Melbourne
#  Semester 1, 2017
#  by: Ammar Ahmed
#  Username: ammara
#

CC = gcc
CFLAGS = -Wall -Wextra -std=gnu99

## OBJ = Object files.
## SRC = Source files.
## EXE = Executable name.

SRC = server.c
OBJ = server.o
EXE = server

## Top level target is executable.
$(EXE):	$(OBJ)
	$(CC) $(CFLAGS) -o $(EXE) $(OBJ) -lm -lpthread


## Clean: Remove object files and core dump files.
clean:
	rm $(OBJ) 

## Clobber: Performs Clean and removes executable file.
clobber: clean
	rm $(EXE) 

## Dependencies
server.o:	  server.h