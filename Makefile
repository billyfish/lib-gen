#
# Makefile generated by:
# CodeBench 0.23
#
# Project: libgen
#
# Created on: 30-11-2014 15:24:00
#
#

CC = SDK:gcc/bin/gcc
OBJ = \
	 src/byte_buffer.o src/document_parser.o src/library_utils.o \
	 src/header_definitions.o src/idl_writer.o src/main.o \
	 src/utils.o src/parameter.o src/function_definition.o \
	 src/writer.o src/c_writer.o src/makefile_utils.o \
	

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

src/byte_buffer.o: src/byte_buffer.c src/byte_buffer.h src/utils.h
	$(CC) -c src/byte_buffer.c -o src/byte_buffer.o $(CFLAGS)

src/document_parser.o: src/document_parser.c src/document_parser.h src/byte_buffer.h src/utils.h
	$(CC) -c src/document_parser.c -o src/document_parser.o $(CFLAGS)

src/library_utils.o: src/library_utils.c src/library_utils.h
	$(CC) -c src/library_utils.c -o src/library_utils.o $(CFLAGS)

src/header_definitions.o: src/header_definitions.c src/header_definitions.h src/function_definition.h src/parameter.h src/debugging_utils.h src/utils.h src/makefile_utils.h
	$(CC) -c src/header_definitions.c -o src/header_definitions.o $(CFLAGS)

src/idl_writer.o: src/idl_writer.c src/idl_writer.h src/writer.h src/header_definitions.h src/parameter.h src/function_definition.h src/debugging_utils.h
	$(CC) -c src/idl_writer.c -o src/idl_writer.o $(CFLAGS)

src/main.o: src/main.c src/utils.h src/header_definitions.h src/function_definition.h src/parameter.h src/debugging_utils.h src/idl_writer.h src/writer.h src/library_utils.h
	$(CC) -c src/main.c -o src/main.o $(CFLAGS)

src/utils.o: src/utils.c src/debugging_utils.h src/utils.h src/parameter.h src/header_definitions.h src/function_definition.h
	$(CC) -c src/utils.c -o src/utils.o $(CFLAGS)

src/parameter.o: src/parameter.c src/debugging_utils.h src/parameter.h src/utils.h
	$(CC) -c src/parameter.c -o src/parameter.o $(CFLAGS)

src/function_definition.o: src/function_definition.c src/function_definition.h src/parameter.h src/debugging_utils.h src/utils.h
	$(CC) -c src/function_definition.c -o src/function_definition.o $(CFLAGS)

src/writer.o: src/writer.c src/writer.h src/header_definitions.h src/function_definition.h
	$(CC) -c src/writer.c -o src/writer.o $(CFLAGS)

src/c_writer.o: src/c_writer.c src/c_writer.h src/writer.h src/header_definitions.h
	$(CC) -c src/c_writer.c -o src/c_writer.o $(CFLAGS)

src/makefile_utils.o: src/makefile_utils.c src/makefile_utils.h src/utils.h
	$(CC) -c src/makefile_utils.c -o src/makefile_utils.o $(CFLAGS)

