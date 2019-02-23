.DEFAULT_GOAL := all
CC=gcc
CFLAGS=
SRC_DIR=src
OUT_DIR=build
OBJ_DIR=obj
OBJ_TEST=obj/test
TEST_DIR=test
TEST_OUT=test/build
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

$(OBJ_TEST)/%.o: $(TEST_DIR)/%.c
	$(CC) --std=c11 -c $^ -o $@ $(CFLAGS)

build_dir:
ifeq ($(OS),Windows_NT)
	$(call win_mkdir,$(OBJ_DIR))
	$(call create_dir,$(SHARE_OBJ))
	$(call win_mkdir, $(OUT_DIR))
	$(call win_mkdir, $(LIB_DIR))
	$(call win_mkdir, $(OBJ_TEST))
	$(call win_mkdir, $(TEST_OUT))
else
	mkdir -p $(OBJ_DIR)
	mkdir -p $(SHARE_OBJ)
	mkdir -p $(OUT_DIR)
	mkdir -p $(LIB_DIR)
	mkdir -p $(OBJ_TEST)
	mkdir -p $(TEST_OUT)
endif

clean:
ifeq ($(OS),Windows_NT)
	$(rm_win) $(OBJ_DIR)
	$(rm_win) $(LIB_DIR)
	$(rm_win) $(OUT_DIR)

else
	$(rm_unix) $(OBJ_DIR)
	$(rm_unix) $(LIB_DIR)
	$(rm_unix) $(OUT_DIR)
endif

build_all: $(OBJ_DIR)/main.o $(LIB_DIR)/libreg-exp.a
	$(CC) $(OBJ_DIR)/main.o -L$(LIB_DIR) -lreg-exp -o $(OUT_DIR)/test $(CFLAGS)

regexp_test: $(LIB_DIR)/libreg-exp.a $(OBJ_TEST)/test-regex.o
	$(CC) $(OBJ_TEST)/test-regex.o -L$(LIB_DIR) -lreg-exp -o $(TEST_OUT)/test-regexp $(CFLAGS)

lexer_test: $(LIB_DIR)/libreg-exp.a $(OBJ_TEST)/test-lexer.o $(OBJ_DIR)/lexer.o
	$(CC) $(OBJ_TEST)/test-lexer.o $(OBJ_DIR)/lexer.o -L$(LIB_DIR) -lreg-exp -o $(TEST_OUT)/test-lexer $(CFLAGS)

bnf_test: $(LIB_DIR)/libreg-exp.a $(OBJ_TEST)/test-bnf.o $(OBJ_DIR)/lexer.o $(OBJ_DIR)/syntax.o
	$(CC) $(OBJ_TEST)/test-bnf.o $(OBJ_DIR)/lexer.o $(OBJ_DIR)/syntax.o -L$(LIB_DIR) -lreg-exp -o $(TEST_OUT)/test-bnf $(CFLAGS)

syntax_test: $(LIB_DIR)/libreg-exp.a $(OBJ_TEST)/test-syntax.o $(OBJ_DIR)/lexer.o $(OBJ_DIR)/syntax.o
	$(CC) $(OBJ_TEST)/test-syntax.o $(OBJ_DIR)/lexer.o $(OBJ_DIR)/syntax.o -L$(LIB_DIR) -lreg-exp -o $(TEST_OUT)/test-syntax $(CFLAGS)

build_test: regexp_test lexer_test

dir: build_dir

test: build_dir build_test bnf_test syntax_test

run_test: test
	npm test

all: build_dir build_all

debug: 
	make build_all CFLAGS=-g

$(LIB_DIR)/libreg-exp.a: $(SHARE_OBJ)/reg-exp-parser.o $(SHARE_OBJ)/reg-exp-compiler.o $(SHARE_OBJ)/reg-exp-matcher.o $(SHARE_OBJ)/reg-exp.o $(SHARE_OBJ)/error.o $(SHARE_OBJ)/data-struct.o
	ar rcs $@ $^