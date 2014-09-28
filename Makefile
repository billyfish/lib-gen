#
# Makefile generated by:
# CodeBench 0.23
#
# Project: libgen
#
# Created on: 29-09-2014 00:26:51
#
#

CC = SDK:gcc/bin/gcc
OBJ = \
	 src/header_definitions.o src/idl_writer.o src/memory.o \
	 src/main.o src/utils.o src/parameter.o \
	 src/function_definition.o src/writer.o

BIN = libgen

INCPATH = -I.

CFLAGS = $(INCPATH) -Wall -Wextra -Wwrite-strings -D_AMIGA_ -D_DEBUG -gstabs

LDFLAGS = 

LIBS = 
#	add any extra linker libraries you want here

.PHONY: all all-before all-after clean clean-custom

all: all-before $(BIN) all-after

all-before:
#	You can add rules here to execute before the project is built

all-after:
#	You can add rules here to execute after the project is built

clean: clean-custom
	rm $(OBJ) $(BIN)

$(BIN): $(OBJ)
#	Debug builds require the -g or -gstabs option in CFLAGS
#	You may also need to move the LDFLAGS variable depending on the contents
	$(CC) -o $(BIN).debug $(OBJ) $(LDFLAGS) $(LIBS)
	strip $(BIN).debug -o $(BIN)

src/header_definitions.o: src/header_definitions.c src/header_definitions.h src/function_definition.h src/parameter.h src/memory.h src/debugging_utils.h
	$(CC) -c src/header_definitions.c -o src/header_definitions.o $(CFLAGS)

src/idl_writer.o: src/idl_writer.c src/idl_writer.h src/writer.h src/memory.h src/parameter.h src/function_definition.h src/header_definitions.h src/debugging_utils.h
	$(CC) -c src/idl_writer.c -o src/idl_writer.o $(CFLAGS)

src/memory.o: src/memory.c src/memory.h
	$(CC) -c src/memory.c -o src/memory.o $(CFLAGS)

src/main.o: src/main.c src/utils.h src/header_definitions.h src/function_definition.h src/parameter.h src/memory.h src/debugging_utils.h src/idl_writer.h src/writer.h
	$(CC) -c src/main.c -o src/main.o $(CFLAGS)

src/utils.o: src/utils.c src/debugging_utils.h src/memory.h src/utils.h src/parameter.h src/header_definitions.h src/function_definition.h
	$(CC) -c src/utils.c -o src/utils.o $(CFLAGS)

src/parameter.o: src/parameter.c src/debugging_utils.h src/memory.h src/parameter.h src/utils.h
	$(CC) -c src/parameter.c -o src/parameter.o $(CFLAGS)

src/function_definition.o: src/function_definition.c src/function_definition.h src/parameter.h src/debugging_utils.h src/memory.h
	$(CC) -c src/function_definition.c -o src/function_definition.o $(CFLAGS)

src/writer.o: src/writer.c src/writer.h
	$(CC) -c src/writer.c -o src/writer.o $(CFLAGS)

