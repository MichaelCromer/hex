CC = gcc
CFLAGS = -Wall -Wextra -pedantic -g
CLIBS = -lncurses -lm

SRC_DIR = ./src
HDR_DIR = $(SRC_DIR)/hdr
BLD_DIR = ./bld
OBJ_DIR = $(BLD_DIR)/obj
TARGET = hex

SRC = $(wildcard $(SRC_DIR)/*.c)
HDR = $(wildcard $(HDR_DIR)/*.h)
OBJ = $(SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

$(BLD_DIR)/$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $@ $(CLIBS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR) $(BLD_DIR):
	mkdir -p $@

clean: ; rm -rf $(OBJ_DIR)
.PHONY: clean

tags: ; ctags $(SRC) $(HDR)
.PHONY: tags
