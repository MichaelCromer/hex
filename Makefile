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
	rm -r $(BLDDIR)
