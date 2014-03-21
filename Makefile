CC = gcc
CFLG = -c -g -Wall
LDFLG = -lpthread
SRC = main.c myh3c.c lib/daemon.c user/user.c
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
