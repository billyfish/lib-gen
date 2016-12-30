#
# Makefile generated by:
# CodeBench 0.42
#
# Project: libgen
#
# Created on: 30-12-2016 17:20:48
#
#

CC = SDK:gcc/bin/gcc
LD = SDK:gcc/bin/gcc
OBJ = \
	 src/auto_init_writer.o src/init_writer.o src/inline_header_writer.o \
	 src/makefile_writer.o src/proto_header_writer.o src/list_utils.o \
	 src/byte_buffer.o src/document_parser.o src/library_utils.o \
	 src/idl_writer.o src/main.o src/utils.o \
	 src/parameter.o src/function_definition.o src/writer.o \
	 src/c_writer.o src/makefile_utils.o src/vectors.o \
	 src/interface_h_writer.o

BIN = libgen

INCPATH = -I.

CFLAGS = $(INCPATH) -Wall -Wextra -Wwrite-strings -D_AMIGA_  -gstabs

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

auto_init_writer.o: src/auto_init_writer.c src/auto_init_writer.h src/utils.h src/debugging_utils.h

init_writer.o: src/init_writer.c src/debugging_utils.h src/utils.h src/init_writer.h

inline_header_writer.o: src/inline_header_writer.c src/inline_header_writer.h src/debugging_utils.h src/function_definition.h src/parameter.h src/utils.h

makefile_writer.o: src/makefile_writer.c src/makefile_writer.h src/function_definition.h src/parameter.h src/utils.h src/debugging_utils.h src/makefile_utils.h

proto_header_writer.o: src/proto_header_writer.c src/proto_header_writer.h src/debugging_utils.h src/utils.h

list_utils.o: src/list_utils.c src/list_utils.h src/debugging_utils.h

byte_buffer.o: src/byte_buffer.c src/byte_buffer.h src/utils.h src/debugging_utils.h

document_parser.o: src/document_parser.c src/document_parser.h src/byte_buffer.h src/utils.h src/debugging_utils.h

library_utils.o: src/library_utils.c src/library_utils.h src/debugging_utils.h

idl_writer.o: src/idl_writer.c src/idl_writer.h src/writer.h src/function_definition.h src/parameter.h src/debugging_utils.h src/utils.h

main.o: src/main.c src/utils.h src/function_definition.h src/parameter.h src/document_parser.h src/byte_buffer.h src/debugging_utils.h src/idl_writer.h src/writer.h src/inline_header_writer.h src/auto_init_writer.h src/init_writer.h src/proto_header_writer.h src/interface_h_writer.h src/library_utils.h src/list_utils.h src/makefile_writer.h src/vectors.h

utils.o: src/utils.c src/debugging_utils.h src/utils.h src/parameter.h src/function_definition.h

parameter.o: src/parameter.c src/debugging_utils.h src/parameter.h src/utils.h

function_definition.o: src/function_definition.c src/function_definition.h src/parameter.h src/debugging_utils.h src/utils.h

writer.o: src/writer.c src/writer.h src/function_definition.h src/parameter.h src/debugging_utils.h

c_writer.o: src/c_writer.c src/c_writer.h src/writer.h src/function_definition.h src/debugging_utils.h

makefile_utils.o: src/makefile_utils.c src/makefile_utils.h src/function_definition.h src/parameter.h src/utils.h src/debugging_utils.h

vectors.o: src/vectors.c src/function_definition.h src/parameter.h src/debugging_utils.h src/utils.h

interface_h_writer.o: src/interface_h_writer.c src/interface_h_writer.h src/utils.h src/debugging_utils.h src/function_definition.h src/parameter.h

