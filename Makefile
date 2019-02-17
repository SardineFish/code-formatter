.DEFAULT_GOAL := all
CC=gcc
CFLAGS=
SRC_DIR=src
OUT_DIR=build
OBJ_DIR=obj
SHARE_OBJ=obj/shared
LIB_DIR=lib
rm_win=rd /S /Q
rm_unix=rm -r

define win_mkdir
	if not exist "$(abspath $(1))" mkdir "$(abspath $(1))"
endef

define create_dir
	$(if ($(OS),Windows_NT), $(call win_mkdir,$(1)), mkdir -p $(1))
endef

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
# $(call create_dir,$(@D))
	$(CC) --std=c11 -c $^ -o $@ $(CFLAGS)

$(SHARE_OBJ)/%.o: $(SRC_DIR)/%.c
# $(call create_dir,$(@D))
	$(CC) --std=c11 -c -fPIC $^ -o $@ $(CFLAGS)

build_dir:
ifeq ($(OS),Windows_NT)
	$(call win_mkdir,$(OBJ_DIR))
	$(call create_dir,$(SHARE_OBJ))
	$(call win_mkdir, $(OUT_DIR))
	$(call win_mkdir, $(LIB_DIR))
else
	mkdir -p $(OBJ_DIR)
	mkdir -p $(SHARE_OBJ)
	mkdir -p $(OUT_DIR)
	mkdir -p $(LIB_DIR)
endif

clean:
ifeq ($(OS),Windows_NT)
	$(rm_win) $(OBJ_DIR)
	$(rm_win) $(LIB_DIR)
else
	$(rm_unix) $(OBJ_DIR)
	$(rm_unix) $(LIB_DIR)
endif

build_all: $(OBJ_DIR)/main.o $(LIB_DIR)/libreg-exp.a
	$(CC) $(OBJ_DIR)/main.o -L$(LIB_DIR) -lreg-exp -o $(OUT_DIR)/test $(CFLAGS)
	

all: build_dir build_all

debug: 
	make build_all CFLAGS=-g

$(LIB_DIR)/libreg-exp.a: $(SHARE_OBJ)/reg-exp-parser.o $(SHARE_OBJ)/reg-exp-compiler.o $(SHARE_OBJ)/reg-exp-matcher.o $(SHARE_OBJ)/reg-exp.o $(SHARE_OBJ)/error.o $(SHARE_OBJ)/data-struct.o
	ar rcs $@ $^