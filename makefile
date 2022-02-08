CC=gcc
EXEC=./test.out
CFLAGS=-Wunused-function -Wall -o ${EXEC} -DTERM_COLORS


all : test

quiet:
	gcc ${CFLAGS} test.c -DQUIET
	@${EXEC}

test:
	@echo Compiling tests...
	gcc ${CFLAGS} test.c
	@printf "\nRunning tests...\n"
	@${EXEC}
