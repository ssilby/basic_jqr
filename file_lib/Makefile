CC=gcc
CFLAGS += -Wall -Wextra -Wpedantic
CFLAGS += -Wvla -Wwrite-strings -Waggregate-return -Wfloat-equal
CFLAGS += -std=c11
CPPFLAGS += -D_POSIX_C_SOURCE=200809L
file_lib: file_lib.o

test/test_all: LDLIBS += -lcheck -lrt -lsubunit -lpthread -lm
test/test_all: test/test_all.o test/test_file_lib.o file_lib.o 


.PHONY: debug
debug: CFLAGS += -g
debug: driver

.PHONY: check
check: test/test_all
	./test/test_all

.PHONY: clean
clean:
	$(RM) *.o driver test/*.o test/test_all
