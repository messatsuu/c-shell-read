# PHONY are commands that are not files
.PHONY: build

# GENERAL
CC = clang
TARGET = build/libcshread.a

# FLAGS
CFLAGS = -std=c23 -Wall -Werror -I$(INC_DIR) -D_POSIX_C_SOURCE=200809L
DEBUG_CFLAGS = -std=c23 -g -O0 -D_POSIX_C_SOURCE=200809L

# DIRS
SRC_DIR = ./src
INC_DIR = ./include
BUILD_DIR = ./build

# FILES
SRC_FILES := $(shell find $(SRC_DIR) -name '*.c')
OBJ_FILES := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRC_FILES))

# TARGETS
build: $(TARGET)

# Build the static library from all object files
$(TARGET): $(OBJ_FILES)
	@mkdir -p $(dir $@)
	ar rcs $@ $^

# Compile each .c to .o in build/
build/%.o: src/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Clean build artifacts
clean:
	rm build -rf

# ALIASES
b: build
