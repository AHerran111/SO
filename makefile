# Compiler
CC = gcc

# Compiler flags (optional: -Wall shows warnings, -g adds debug info)
CFLAGS = -Wall -Wextra -g

# Target executable name
TARGET = a.out

# Default rule: build the program
all: $(TARGET)

# Rule to build the target
$(TARGET): shell.c
	$(CC) $(CFLAGS) -o $(TARGET) shell.c

# Clean up compiled files
clean:
	rm -f $(TARGET)
