CC = gcc
CFLG = -std=c99 -c -g -Wall
LDFLG =
SRC = main.c myh3c.c lib/daemon.c lib/shortcuts/struct.c user/user.c
OBJ = $(SRC:.c=.o)
EXE = myh3c

$(EXE): $(OBJ) 
	$(CC) $(OBJ) $(LDFLG) -o $@

%.o: %.c
	$(CC) $(CFLG) $? -o $@

run:
	sudo ./${EXE}

clean:
	rm ${OBJ} ${EXE}
