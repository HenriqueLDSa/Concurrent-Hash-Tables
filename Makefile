CC = gcc
CFLAGS = -Wall -lpthread
CHASH = chash
RM = rm -f

# Put all file names here with .o after
OBJECTS = chash.o

.PRECIOUS: $(CHASH)
.PHONY: output

output: chash 
	./chash

chash: chash.o
	$(CC) $(CFLAGS) -o $(CHASH) $(OBJECTS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) *.o $(CHASH) output.txt