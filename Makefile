CC = gcc
CFLAGS = -Wall
LDFLAGS = -lpthread
CHASH = chash
RM = rm -f

OBJECTS = chash.o hash_table.o thread_functions.o rwlock.o utils.o

.PHONY: all clean run

all: $(CHASH)

$(CHASH): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(CHASH) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) *.o $(CHASH) output.txt

run: all
	./$(CHASH)
