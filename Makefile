#
# Makefile generated by:
# CodeBench 0.42
#
# Project: libgen
#
# Created on: 29-08-2015 17:31:28
#
#

CC = SDK:gcc/bin/gcc
LD = SDK:gcc/bin/gcc
OBJ = \
	 src/byte_buffer.o src/document_parser.o src/library_utils.o \
	 src/header_definitions.o src/idl_writer.o src/main.o \
	 src/utils.o src/parameter.o src/function_definition.o \
	 src/writer.o src/c_writer.o src/makefile_utils.o \
	 src/module_utils.o

BIN = libgen

INCPATH = -I.

CFLAGS = $(INCPATH) -Wall -Wextra -Wwrite-strings -D_AMIGA_  -D_DEBUG -gstabs

LDFLAGS = 

LIBS = 
#	add any extra linker libraries you want here

.PHONY: all all-before all-after clean clean-custom realclean

all: all-before $(BIN) all-after

all-before:
#	You can add rules here to execute before the project is built

all-after:
#	You can add rules here to execute after the project is built

clean: clean-custom
	rm -v $(OBJ)

realclean:
	rm -v $(OBJ) $(BIN)

$(BIN): $(OBJ) $(LIBS)
#	You may need to move the LDFLAGS variable in this rule depending on its contents
	@echo "Linking $(BIN)"
	@$(LD) -o $(BIN).debug $(OBJ) $(LDFLAGS) $(LIBS)
	strip $(BIN).debug -o $(BIN)

###################################################################
##
##  Standard rules
##
###################################################################

# A default rule to make all the objects listed below
# because we are hiding compiler commands from the output

.c.o:
	@echo "Compiling $<"
	@$(CC) -c $< -o $*.o $(CFLAGS)

byte_buffer.o: src/byte_buffer.c src/byte_buffer.h src/utils.h src/debugging_utils.h

document_parser.o: src/document_parser.c src/document_parser.h src/byte_buffer.h src/utils.h src/debugging_utils.h

library_utils.o: src/library_utils.c src/library_utils.h

header_definitions.o: src/header_definitions.c src/header_definitions.h src/function_definition.h src/parameter.h src/debugging_utils.h src/utils.h src/makefile_utils.h

idl_writer.o: src/idl_writer.c src/idl_writer.h src/writer.h src/header_definitions.h src/parameter.h src/function_definition.h src/debugging_utils.h

main.o: src/main.c src/utils.h src/header_definitions.h src/function_definition.h src/parameter.h src/document_parser.h src/byte_buffer.h src/debugging_utils.h src/idl_writer.h src/writer.h src/library_utils.h

utils.o: src/utils.c src/debugging_utils.h src/utils.h src/parameter.h src/header_definitions.h src/function_definition.h

parameter.o: src/parameter.c src/debugging_utils.h src/parameter.h src/utils.h

function_definition.o: src/function_definition.c src/function_definition.h src/parameter.h src/debugging_utils.h src/utils.h

writer.o: src/writer.c src/writer.h src/header_definitions.h src/function_definition.h

c_writer.o: src/c_writer.c src/c_writer.h src/writer.h src/header_definitions.h

makefile_utils.o: src/makefile_utils.c src/makefile_utils.h src/utils.h

module_utils.o: src/module_utils.c

