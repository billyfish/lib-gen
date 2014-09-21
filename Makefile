NAME 	= libgen
BUILD = debug

DIR_OBJ = build-$(BUILD)
COMP	= gcc


VPATH	= \
	src
	
INCLUDES = \
	src
	
SRCS 	= \
	main.c \
	memory.c \
	parameter.c \
	utils.c 

OBJS = $(patsubst %.c, $(DIR_OBJS)/%.o, $(SRCS))	

all: $(OBJS)
	
ifeq ($(BUILD),release)
	CFLAGS 	+= -O3 -s
 	LDFLAGS += -s
else
#	CFLAGS 	+= -g
	CFLAGS 	+= -g -pg
	LDFLAGS += -pg
	CPPFLAGS += -D_DEBUG
endif

.PHONY:	all

all: lib exe

all: $(OBJS)
	$(COMP) $(CFLAGS) $(OBJS) -o $(BUILD)/$(NAME)



	
