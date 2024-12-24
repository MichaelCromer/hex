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

.PHONY: clean
clean:
	rm -rf $(BLDDIR)
	make


format:
	indent -npro -linux -l88 -nut -i4 -cli4 src/*.c src/include/*.h
	find -name '*~' -delete
	sed -i '/ + \| - /s/ \([*\/]\) /\1/g' src/*.c src/include/*.h
