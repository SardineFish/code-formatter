.DEFAULT_GOAL := all
CC=g++
CFLAGS=
SRC_DIR=src
OUT_DIR=build
OBJ_DIR=obj
rm_win=del /q
rm_unix=rm -r

define win_mkdir
	if not exist $(1) mkdir $(1)
endef

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CC) -c $^ -o $@ $(CFLAGS)

build_dir:
ifeq ($(OS),Windows_NT)
	$(call win_mkdir, $(OBJ_DIR))
	$(call win_mkdir, $(OUT_DIR))
else
	mkdir -p $(OBJ_DIR)
	mkdir -p $(OUT_DIR)
endif

clean:
ifeq ($(OS),Windows_NT)
	$(rm_win) $(OBJ_DIR)\*
else
	$(rm_unix) $(OBJ_DIR)/*
endif

all: $(OBJ_DIR)/main.o build_dir
	$(CC) -o $(OUT_DIR)/test $(OBJ_DIR)/main.o $(CFLAGS)
