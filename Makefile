CC = gcc
CFLAGS = -Wall -Wextra -pedantic -g
CLIBS = -lncurses -lm

SRCDIR = src
BLDDIR = build
TARGET = hex

SRC = $(wildcard $(SRCDIR)/*.c)
OBJ = $(SRC:%.c=$(BLDDIR)/%.o)

$(BLDDIR)/$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $@ $(CLIBS)

$(BLDDIR)/%.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BLDDIR)
	make
.PHONY: clean

format:
	indent -npro -linux -l88 -nut -i4 -cli4 src/*.c src/include/*.h
	find -name '*~' -delete
	sed -i '/ + \| - /s/ \([*\/]\) /\1/g' src/*.c src/include/*.h
.PHONY: format

tags:
	ctags src/*.c src/include/*.h
