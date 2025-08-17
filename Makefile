CC = gcc
CFLAGS = -Wall -Wextra -Wpedantic -Werror
CLIBS = -lncurses -lm

DIR_SRC = ./src
DIR_HDR = $(DIR_SRC)/hdr
DIR_BLD = ./bld
DIR_OBJ = $(DIR_BLD)/obj
TARGET = hex

SRC = $(wildcard $(DIR_SRC)/*.c)
HDR = $(wildcard $(DIR_HDR)/*.h)
OBJ = $(SRC:$(DIR_SRC)/%.c=$(DIR_OBJ)/%.o)


$(DIR_BLD)/$(TARGET): $(OBJ) | $(DIR_BLD)
	$(CC) $(CFLAGS) $(OBJ) -o $@ $(CLIBS)


$(DIR_OBJ)/%.o: $(DIR_SRC)/%.c | $(DIR_OBJ)
	$(CC) $(CFLAGS) -c $< -o $@


$(DIR_OBJ) $(DIR_BLD): ; mkdir -p $@


.PHONY: dev
dev : CFLAGS += -g -fsanitize=address,leak,undefined
dev : clean $(DIR_BLD)/$(TARGET)


.PHONY: clean
clean: ; rm -rf $(DIR_OBJ)


.PHONY: tags
tags: ; ctags $(SRC) $(HDR)
