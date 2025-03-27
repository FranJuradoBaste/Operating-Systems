# NO TOCAR / NOT MODIFIED ME ##
CC=gcc
FLAGS=-Wno-implicit-function-declaration
CFLAGS=-I.
###############################

# MODIFIED ME ##

OBJ = scripter.o 

all: scripter

%.o: %.c 
	$(CC) $(FLAGS) -c -o $@ $< $(CFLAGS)

scripter: $(OBJ)
	$(CC) $(FLAGS) -L. -o $@ $< $(LIBS)

clean:
	rm -f ./scripter.o ./scripter
