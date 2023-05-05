ODIR=build
IDIR=include
SDIR=src


CC=gcc
CFLAGS+= -I$(IDIR) -g -std=c99 -D_POSIX_C_SOURCE=200809L -Wall -Wextra -Wpedantic -Waggregate-return -Wwrite-strings -Wfloat-equal -Wvla

all: file_server

_DEPS = server.h file_lib.h my_ht.h netlib.h pool_queue.h
DEPS=$(patsubst %,$(IDIR)/%,$(_DEPS))
_OBJ = server.o file_lib.o my_ht.o netlib.o pool_queue.o server_drive.o 
OBJ=$(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: $(SDIR)/%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

file_server: $(OBJ)
	$(CC) $(CFLAGS) -o bin/$@ $^ -lpthread

.PHONY: clean
clean:
	$(RM) -r bin/file_server build/*.o
