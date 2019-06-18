TARGET = prog
LIBS = -lm
CC = gcc
CFLAGS = -Wall -Wextra
CFLAGS_OPT = -O3

.PHONY: default all clean debug cleandebug

default: $(TARGET)
all: default

OBJECTS = $(patsubst %.c, %.o, $(wildcard *.c))
HEADERS = $(wildcard *.h)

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) $(CFLAGS_OPT) -c $< -o $@

.PRECIOUS: $(TARGET) $(OBJECTS)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -Wall $(LIBS) -o $@

clean:	cleandebug
	-rm -f *.o
	-rm -f $(TARGET)


#DEBUG

CFLAGS_DEBUG = -ggdb3

OBJECTS_DEBUG = $(patsubst %.c, %.o.debug, $(wildcard *.c))

debug: $(TARGET).debug

$(TARGET).debug: $(OBJECTS_DEBUG)
	$(CC) $(OBJECTS_DEBUG) -Wall $(LIBS) -o $@

%.o.debug: %.c $(HEADERS)
	$(CC) $(CFLAGS) $(CFLAGS_DEBUG) -c $< -o $@

cleandebug:
	-rm -f *.debug
