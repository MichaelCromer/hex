########################################################################################
#
#	hex Makefeile
#
########################################################################################


#=======================================================================================
#	Setup

DIR_SRC = ./src
DIR_HDR = $(DIR_SRC)/hdr
DIR_BLD = ./bld
DIR_OBJ = $(DIR_BLD)/obj
DIR_DEP = $(DIR_BLD)/dep

TARGET = $(DIR_BLD)/hex

SRC = $(wildcard $(DIR_SRC)/*.c)
HDR = $(wildcard $(DIR_HDR)/*.h)
OBJ = $(SRC:$(DIR_SRC)/%.c=$(DIR_OBJ)/%.o)
DEP = $(SRC:$(DIR_SRC)/%.c=$(DIR_DEP)/%.d)

CC = gcc
FLAG_C = -Wall -Wextra -Wpedantic -Werror
FLAG_D = -MT $@ -MMD -MP -MF $(DIR_DEP)/$*.d
LIB_C = -lncurses -lm


#=======================================================================================
#	Build (compile/link)

$(TARGET): $(OBJ) | $(DIR_BLD)
	$(CC) $(FLAG_C) $(OBJ) -o $@ $(LIB_C)


$(DIR_OBJ)/%.o: $(DIR_SRC)/%.c $(DIR_DEP)/%.d | $(DIR_OBJ) $(DIR_DEP)
	$(CC) $(FLAG_C) $(FLAG_D) -c $< -o $@


#=======================================================================================
#	Structural

include $(wildcard $(DEP))

$(DEP) : ;


#=======================================================================================
#	Directories

$(DIR_OBJ) $(DIR_BLD) $(DIR_DEP): ; mkdir -p $@


.PHONY: clean
clean: ; rm -f $(TARGET) $(OBJ) $(DEP)


#=======================================================================================
#	Utility

.PHONY: dev
dev : FLAG_C += -g -fsanitize=address,leak,undefined
dev : clean $(TARGET)

.PHONY: tags
tags: ; ctags $(SRC) $(HDR)
