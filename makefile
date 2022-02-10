CC=gcc
EXEC=./tst/test.out
CFLAGS=-Wunused-function -Wall -o ${EXEC} -DTERM_COLORS

# Test cases:
all : int string

int:
	gcc ${CFLAGS} tst/test_int.c
	@echo ==== INT ====
	@${EXEC}
	@echo ^^^^ INT ^^^^

string:
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
