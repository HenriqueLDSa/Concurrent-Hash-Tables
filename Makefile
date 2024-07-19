CC = gcc
CFLAGS = -Wall
CHASH = chash
RM = rm -f

# Put all file names here with .o after
OBJECTS = chash.o

.PRECIOUS: $(CHASH)

chash: chash.o
	$(CC) $(CFLAGS) -o $(CHASH) $(OBJECTS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) *.o $(CHASH)