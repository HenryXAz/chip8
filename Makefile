CC := gcc

CFLAGS := \
	-std=c17 \
	-Wall \
	-Wextra \
	-Wpedantic \
	-Wconversion \
	-Wshadow \
	-MMD \
	-MP

SDL_FLAGS := $(shell pkg-config -cflags sdl3)
SDL_LIBS := $(shell pkg-config --libs sdl3)

SRC_DIR := src
BUILD_DIR := build

TARGET := chip8

SOURCES := \
	$(SRC_DIR)/main.c \
	$(SRC_DIR)/chip8.c \
	$(SRC_DIR)/frontend.c 

OBJECTS := $(SOURCES:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

DEPS := $(OBJECTS:.o=.d)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) $(SDL_LIBS) -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) $(SDL_FLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	rm -rf $(BUILD_DIR) $(TARGET)

-include $(DEPS)

debug: CFLAGS += -g -O0
debug: clean $(TARGET)

.PHONY: all run clean











