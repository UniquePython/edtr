CC      = gcc
CFLAGS  = -Wall -Wextra -Wpedantic -std=c11 -I include
LIBS    = -lm

SRC_DIR   = src
BUILD_DIR = build

SRCS   = $(wildcard $(SRC_DIR)/*.c)
OBJS   = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SRCS))
TARGET = $(BUILD_DIR)/edtr

.PHONY: all clean run debug

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

run: all
	./$(TARGET) $(ARGS)

debug: CFLAGS += -g -O0
debug: all
	gdb --args ./$(TARGET) $(ARGS)

clean:
	rm -f $(BUILD_DIR)/*.o $(TARGET)