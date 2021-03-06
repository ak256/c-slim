CC = gcc
TARGET = cslim_compiler
DEBUG_FLAGS = -g
FLAGS = -Wall -Wno-parentheses
LINK_FLAGS = $(FLAGS)
OBJECTS = $(patsubst %.c, %.o, $(shell find . -name "*.c"))

.SILENT:

all: $(TARGET)

debug: $(FLAGS) += $(DEBUG_FLAGS)
debug: $(TARGET)
	gdb --args $(TARGET)

test:
	./$(TARGET) --version test.cslim

$(TARGET): $(OBJECTS)
	$(CC) -o $@ $(OBJECTS) $(LINK_FLAGS)

%.o: %.c
	$(CC) $(FLAGS) -c $^ -o $@

clean:
	rm $(TARGET) $(OBJECTS)
