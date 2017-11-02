PROJECT = myshell
HEADERS = $(PROJECT).h
OBJ = myshell.o parser.o globals.o execute.o

C99 = gcc -std=c99
CFLAGS = -Wall -pedantic -Werror

$(PROJECT) : $(OBJ)
	$(C99) $(CFLAGS) -o $(PROJECT) $(OBJ) -lm
%.o : %.c $(HEADERS)
	$(C99) $(CFLAGS) -c $<
clean:
	rm -f $(PROJECT) $(OBJ)
