#  Project 2 for COMP30023: Computer Systems
#  at the University of Melbourne
#  Semester 1, 2017
#  by: Ammar Ahmed
#  Username: ammara
#

CC = gcc
CFLAGS = -Wall -Wextra -std=gnu99 -O3

## OBJ = Object files.
## SRC = Source files.
## EXE = Executable name.

SRC = server.c sha256.c mine.c input-handler.c list.c
OBJ = server.o sha256.o mine.o input-handler.o list.o
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
	scp *.c *.h Makefile digitalis2:COMP30023/project2

## Submitting the project
submit:
	submit comp30023 project2 *.c *.h Makefile

## Dependencies
server.o:	  	 	server.h uint256.h sha256.h list.h
sha256.o:	  	 	sha256.h
mine.o:		  	 	mine.h uint256.h sha256.h
input-handler.o: 	input-handler.h mine.h server.h sha256.h list.h
list.o:				list.h
