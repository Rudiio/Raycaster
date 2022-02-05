CC = gcc
CFLAGS = -std=c11 -Wall -ggdb3
LIBFLAGS = -lm -lSDL2 -lSDL2main
SRC = raycaster.c
COMPILE = $(CC) $(CFLAGS) -o $@ $(SRC) $(LIBFLAGS)

all : raycaster

raycaster : $(SRC)
	$(CC) $(CFLAGS) -o $@ $(SRC) $(LIBFLAGS)

clean :
	rm raycaster
