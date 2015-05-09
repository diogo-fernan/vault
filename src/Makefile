# Makefile

EXEC = ../bin/vault

BDIR = ../bin
IDIR = ../include
SDIR = ../src
ODIR = obj

CC = gcc
CFLAGS = -Wall -I$(IDIR)

_DEPS = aes.h cli.h crypto.h db.h md5.h sqlite3.h
DEPS  = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = aes.o cli.o crypto.o db.o md5.o sqlite3.o vault.o
OBJ  = $(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: %.c $(DEPS)
	$(CC) -g -c -Wall -o $@ $< $(CFLAGS)

$(EXEC): $(OBJ)
	$(CC) -g -Wall -o $@ $^ $(CFLAGS)

.PHONY: clean

run:
	./$(EXEC) $(h)

clean:
	rm -f *~ $(ODIR)/*.o $(IDIR)/*~ 
	rm -f $(EXEC)
