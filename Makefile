.DEFAULT_GOAL := all
CC=g++
CFLAGS=
SRC_DIR=./src
OUT_DIR=./build
OBJ_DIR=obj
win_mkdir=if not exist $(OUT_DIR) mkdir $(OUT_DIR)
rm_win=del /q
rm_unix=rm -r


$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CC) -c $^ -o $@ $(CFLAGS)

out_dir:
ifeq ($(OS),Windows_NT)
	$(win_mkdir)
else
	mkdir -p $(OUT_DIR)
endif

clean:
ifeq ($(OS),Windows_NT)
	$(rm_win) $(OBJ_DIR)\*
else
	$(rm_unix) $(OBJ_DIR)/*
endif

all: $(OBJ_DIR)/main.o out_dir
	$(CC) -o $(OUT_DIR)/test $(OBJ_DIR)/main.o $(CFLAGS)
