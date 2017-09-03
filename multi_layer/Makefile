CC = gcc
CFLAGS = -O4
INC = -I include
INC_PATH = include

a.out: main.o header.o structure.o operation.o timecount.o
	${CC} main.o header.o structure.o operation.o timecount.o ${CFLAGS} ${INC} -o a.out
main.o: main.c ${INC_PATH}/header.h ${INC_PATH}/structure.h ${INC_PATH}/operation.h ${INC_PATH}/timecount.h
	${CC} main.c ${CFLAGS} ${INC} -c
header.o: header.c ${INC_PATH}/header.h ${INC_PATH}/timecount.h
	${CC} header.c ${CFLAGS} ${INC} -c
structure.o: structure.c ${INC_PATH}/header.h ${INC_PATH}/structure.h ${INC_PATH}/timecount.h
	${CC} structure.c ${CFLAGS} ${INC} -c 
operation.o: operation.c ${INC_PATH}/header.h ${INC_PATH}/structure.h ${INC_PATH}/operation.h ${INC_PATH}/timecount.h 
	${CC} operation.c ${CFLAGS} ${INC} -c
timecount.o: timecount.c ${INC_PATH}/timecount.h 
	${CC} timecount.c ${CFLAGS} ${INC} -c

clean:
	@rm -rf *.o a.out
