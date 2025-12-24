# PHONY are commands that are not files
.PHONY: build

# GENERAL
CC = clang
TARGET = build/libcshread.a

# DIRS
SRC_DIR = ./src
INC_DIR = ./include
BUILD_DIR = ./build

# FILES
SRC_FILES := $(shell find $(SRC_DIR) -name '*.c')
OBJ_FILES := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRC_FILES))

# FLAGS
MODE ?= release
ifeq ($(MODE),debug)
	CFLAGS := -std=c23 -g -O0 -I$(INC_DIR) -D_POSIX_C_SOURCE=200809L
else
	CFLAGS = -std=c23 -Wall -Werror -I$(INC_DIR) -D_POSIX_C_SOURCE=200809L \
			-Wpedantic -Wformat=2 -Wno-unused-parameter \
			-Wshadow -Wwrite-strings -Wstrict-prototypes -Wold-style-definition \
			-Wredundant-decls -Wnested-externs -Wmissing-include-dirs
endif


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
	rm build -r

# ALIASES
b: build
c: clean
