CC=gcc
CFLAGS += -Wall -Wextra -Wpedantic
CFLAGS += -Wvla -Wwrite-strings -Waggregate-return -Wfloat-equal
CFLAGS += -std=c11
DEPS = pqueue.h
OBJ = driver.o pqueue.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

driver: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: debug
debug: CFLAGS += -g
debug: driver

.PHONY: clean
clean:
	$(RM) *.o driver
