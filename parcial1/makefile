# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -Wextra -g

# Linker flags
LDLIBS = -lm

# Target executable name
TARGET = a.out

# Default rule: build the program
all: $(TARGET)

# Rule to build the target
$(TARGET): primes.c
	$(CC) $(CFLAGS) -o $(TARGET) primes.c $(LDLIBS)

# Clean up compiled files
clean:
	rm -f $(TARGET)
