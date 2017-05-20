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

SRC = server.c sha256.c
OBJ = server.o sha256.o
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

## Copying source files to both the servers
scp:
	scp -i ~/Documents/COMP30023/labs/default.pem *.c *.h Makefile ec2-user@115.146.93.36:COMP30023
	scp *.c *.h Makefile digitalis:COMP30023/project2

## Dependencies
server.o:	  server.h uint256.h sha256.h
sha256.o:	  sha256.h