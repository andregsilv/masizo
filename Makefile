CFLAGS = -std=c11
LIBS = -lgmp

all: masizo

masizo: compress.o main.o
	${CC} compress.o main.o -o masizo ${CFLAGS} ${LIBS}

run: masizo
	./masizo input output
	./masizo --decompress output re-input
	cmp input re-input
