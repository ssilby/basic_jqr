CFLAGS +=-std=c11 -Wall -Wextra -Wpedantic -Waggregate-return -Wwrite-strings -Wfloat-equal -Wvla
LIBS = stack.o

.DEFAULT_GOAL: driver

driver: $(LIBS)


.PHONY: debug
debug: CFLAGS +=-g
debug: .DEFAULT_GOAL

.PHONY: profile
profile: CFLAGS +=-pg
profile: LDFLAGS +=-pg
profile: .DEFAULT_GOAL

.PHONY: clean
clean:
	$(RM) *.o driver
