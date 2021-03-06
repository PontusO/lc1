#
# Makefile for the lc1 project
#

# Set up basic variables:
CC         = gcc
CFLAGS     = -c -Wall
LDFLAGS    =

# List of sources:
SOURCES    = lc1.c serial.c main.c tpl.c
OBJECTS    = $(SOURCES:.c=.o)

# Name of executable target:
EXECUTABLE = pelletctrld

# Make sure dbus is online
CFLAGS  += $(shell pkg-config --cflags glib-2.0)
LDFLAGS += $(shell pkg-config --libs glib-2.0)

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)

.c.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm $(OBJECTS) $(EXECUTABLE)
