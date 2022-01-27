OUTDIR		:= build
OUTFILE		:= run
SRC			:= ./src
INCLUDE		:= ${SRC}

#objects = ${SRC}/main.c ${SRC}/include/windows.c ${SRC}/include/arena.c ${SRC}/include/player.h
#gcc -g3 -Wall ${objects} -lncurses -o ./${OUTDIR}/${OUTFILE}

default:
	@echo "Choose operation:"
	@echo "		make build_run	- build and run the game"
	@echo "		make build		- build the game"
	@echo "		make rebuild	- rebuild the game"
	@echo "		make run		- run the game"
	@echo "		make clean		- delete all files"

rebuild: clean build

build:
	mkdir -p ${OUTDIR}
	gcc -g3  -I${SRC}/headers ${SRC}/*.c ${SRC}/headers/*.h main.c -lncurses -lrt -lpthread -o ./${OUTDIR}/${OUTFILE}
	
run:
	./${OUTDIR}/${OUTFILE}

build_run : clean build run

valgrind: build
	valgrind --leak-check=full --show-leak-kinds=all -s ./${OUTDIR}/${OUTFILE}

clean: 
	rm -r -f build