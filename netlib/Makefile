CC=gcc
CFLAGS += -Wall -Wextra -Wpedantic
CFLAGS += -Wvla -Wwrite-strings -Wstack-usage=512
CFLAGS += -std=c11
CPPFLAGS += -D_POSIX_C_SOURCE=200809L
server: server.o netlib.o
client: client.o netlib.o

# test/test_all: LDLIBS += -lcheck -lrt -lsubunit -lpthread -lm
# test/test_all: test/test_all.o test/test_file_lib.o file_lib.o 


.PHONY: debug
debug: CFLAGS += -g
debug: server client

#.PHONY: check
#check: test/test_all
#	./test/test_all

.PHONY: clean
clean:
	$(RM) *.o server client

# use for 4.1.19
