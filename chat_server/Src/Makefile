ODIR=build
IDIR=Includes
SDIR=Src


CC=gcc
CFLAGS+= -I$(IDIR) -g -std=c99 -D_POSIX_C_SOURCE=200809L -Wall -Wextra -Wpedantic -Waggregate-return -Wwrite-strings -Wfloat-equal -Wvla

all: chat_server

_DEPS = chat.h file_lib.h my_ht.h netlib.h pool_queue.h
DEPS=$(patsubst %,$(IDIR)/%,$(_DEPS))
_OBJ = chat.o file_lib.o my_ht.o netlib.o pool_queue.o server.o 
OBJ=$(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: $(SDIR)/%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

chat_server: $(OBJ)
	$(CC) $(CFLAGS) -o ../Binaries/$@ $^ -lpthread

.PHONY: clean
clean:
	$(RM) -r ../Binaries/chat_server build/*.o
