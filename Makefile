all:
	mkdir -p build
	gcc -g -Wall -Werror src/main.c -o build/hex -lncurses -lm

clean:
	rm -rf build
