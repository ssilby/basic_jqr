CFLAGS +=-std=c11 -Wall -Wextra -Wpedantic -Waggregate-return -Wwrite-strings -Wfloat-equal -Wvla
LIBS = circle_link.o

.DEFAULT_GOAL: circle_drive

circle_drive: $(LIBS)


.PHONY: debug
debug: CFLAGS +=-g
debug: .DEFAULT_GOAL

.PHONY: profile
profile: CFLAGS +=-pg
profile: LDFLAGS +=-pg
profile: .DEFAULT_GOAL

.PHONY: clean
clean:
	$(RM) *.o circle_drive
