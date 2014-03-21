CC = gcc
CFLG = -c -g -Wall
LDFLG = -lpthread
SRC = main.c myh3c.c
OBJ = $(SRC:.c=.o)
EXE = a.out

$(EXE): $(OBJ) 
	$(CC) $(OBJ) $(LDFLG) -o $@

%.o: %.c
	$(CC) $(CFLG) $? -o $@

run:
	sudo ./${EXE}

clean:
	rm ${OBJ} ${EXE}
