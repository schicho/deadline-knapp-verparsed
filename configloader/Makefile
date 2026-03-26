# Extensible Makefile for configloader
# Usage examples:
#   make
#   make run
#   make clean
#   make CFLAGS='-O0 -g -Wall -Wextra'
#   make LDLIBS='-lm'

CC ?= cc
SRC_DIR ?= .
BUILD_DIR ?= build
TARGET ?= configloader

# Keep user-provided flags extensible via ?= and +=
CPPFLAGS ?=
CFLAGS ?= -std=c11 -O2 -Wall -Wextra
LDFLAGS ?=
LDLIBS ?=

# Auto-discover source files for easy extension.
SRCS := $(wildcard $(SRC_DIR)/*.c)
OBJS := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRCS))
DEPS := $(OBJS:.o=.d)

.PHONY: all clean run debug release help

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -MMD -MP -c $< -o $@

run: $(TARGET)
	./$(TARGET)

# Convenience profiles
# 'make debug' / 'make release'
debug: CFLAGS := -std=c11 -O0 -g -Wall -Wextra
release: CFLAGS := -std=c11 -O2 -DNDEBUG -Wall -Wextra

debug release: clean all

clean:
	rm -rf $(BUILD_DIR) $(TARGET)

help:
	@echo "Targets: all (default), run, clean, debug, release"
	@echo "Variables you can override: CC, TARGET, SRC_DIR, BUILD_DIR, CPPFLAGS, CFLAGS, LDFLAGS, LDLIBS"

-include $(DEPS)
