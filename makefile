CC=gcc
OUT=./tst/test.out
EXEC=${OUT}
CFLAGS=-Wunused-function -fcompare-debug-second -Wall -o ${OUT} -DTERM_COLORS
VGFLAGS=-s --track-origins=yes --leak-check=full

# Test cases:
all : int string

int:
	@echo
	gcc ${CFLAGS} tst/test_int.c
	@echo ==== INT ====
	@${EXEC}
	@echo ^^^^ INT ^^^^

string:
	@echo
	gcc ${CFLAGS} tst/test_string.c
	@echo ==== STRING ====
	@${EXEC}
	@echo ^^^^ STRING ^^^^


# Special rules:
quiet: CFLAGS += -DQUIET
quiet: CFLAGS := $(filter-out -W%,$(CFLAGS))
quiet: all

nocol: CFLAGS := $(filter-out -DTERM_COLORS,$(CFLAGS))
nocol: all

mem: EXEC := valgrind ${VGFLAGS} ${OUT}
mem: all