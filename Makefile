#
# This is makefile for ssearcher
#
# Copyright (c) 2013 by Yang Hong <hy dot styx at gmail dot com>
#

SRCDIR := src
OBJDIR := obj
UNAME_S := $(shell uname -s)

SS_OBJS := $(OBJDIR)/options.o $(OBJDIR)/main.o $(OBJDIR)/match.o $(OBJDIR)/pool.o
SS_SRCS := $(patsubst $(OBJDIR)/%.o,$(SRCDIR)/%.c,$(SS_OBJS))
SS_BIN  := ssearcher

SS_CC=$(QUIET_CC)$(CC)
SS_LD=$(QUIET_LINK)$(CC)
CFLAGS += -O0 -g -Wall

# -pthread is not needed on Darwin
ifneq ($(UNAME_S),Darwin)
LDFLAGS += -pthread
endif


# Borrowed from redis Makefile
# to make output colourful ^_^
CCCOLOR="\033[34m"
LINKCOLOR="\033[34;1m"
SRCCOLOR="\033[33m"
BINCOLOR="\033[37;1m"
MAKECOLOR="\033[32;1m"
ENDCOLOR="\033[0m"

ifndef V
QUIET_CC = @printf '    %b %b\n' $(CCCOLOR)CC$(ENDCOLOR) $(SRCCOLOR)$@$(ENDCOLOR) 1>&2;
QUIET_LINK = @printf '    %b %b\n' $(LINKCOLOR)LINK$(ENDCOLOR) $(BINCOLOR)$@$(ENDCOLOR) 1>&2;
QUIET_INSTALL = @printf '    %b %b\n' $(LINKCOLOR)INSTALL$(ENDCOLOR) $(BINCOLOR)$@$(ENDCOLOR) 1>&2;
endif

# Make sure that 'all' is the first target
all: $(SS_BIN)

.PHONY: all clean depclean

# -include Makefile.dep
# dep: $(SS_SRCS:.c=.d)
# 	@echo "Finish building dependencies" # just do something to make dependencies executed
# .PHONY: dep

# include automatically generated dependencies
-include $(SS_SRCS:.c=.d)

# Generage dependencies like:
# obj/main.o: src/main.c src/options.h src/debug.h
# Make sure the target of dependencies match
# binary files in `obj' directory
# -MT: specify the target file
# -MM: specify input file(s)
%.d: %.c
	$(SS_CC) $(CFLAGS) -MM $< -MT $(patsubst $(SRCDIR)/%.c,$(OBJDIR)/%.o,$<) > $@;


$(OBJDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(@D)
	$(SS_CC) $(CFLAGS) -c -o $@ $(SRCDIR)/$*.c

$(SS_BIN): $(SS_OBJS)
	$(SS_LD) $^ -o $@ $(LDFLAGS)

clean:
	rm -rf $(OBJDIR) $(SS_OBJS)
	rm -rf $(SS_BIN)

depclean:
	rm -rf $(SRCDIR)/*.d


