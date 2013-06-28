#
# This is makefile for ssearcher
#
# Copyright (c) 2013 by Yang Hong <hy dot styx at gmail dot com>
#

SUBDIR := src snippets
SRCDIR := src
OBJDIR := obj
UNAME_S := $(shell uname -s)

SRCS :=
OBJS :=


include $(patsubst %, %/rules.mk, $(SUBDIR))

SS_BIN := ssearcher

include common.mk

# Make sure that 'all' is the first target
default: $(SS_BIN) $(SNPT_BIN)

.PHONY: default clean depclean test

# -include Makefile.dep
# dep: $(SRCS:.c=.d)
# 	@echo "Finish building dependencies" # just do something to make dependencies executed
# .PHONY: dep

# include automatically generated dependencies
-include $(SRCS:.c=.d)

# Generage dependencies like:
# obj/main.o: src/main.c src/options.h src/debug.h
# Make sure the target of dependencies match
# binary files in `obj' directory
# -MT: specify the target file
# -MM: specify input file(s)
%.d: %.c
	$(SS_CC) $(CFLAGS) -MM $< -MT $(patsubst $(SRCDIR)/%.c,$(OBJDIR)/%.o,$<) > $@;

$(OBJDIR)/snippets/%.o: snippets/%.c
	@mkdir -p $(@D)
	$(SS_CC) $(CFLAGS) -Isrc -c -o $@ $<

$(OBJDIR)/%.o: 
	@mkdir -p $(@D)
	$(SS_CC) $(CFLAGS) -c -o $@ $(SRCDIR)/$*.c

$(OBJDIR)/snippets/test_pool: $(OBJDIR)/snippets/test_pool.o $(OBJDIR)/pool.o
	$(SS_LD) -o $@ $^ $(LDFLAGS)

$(SS_BIN): $(OBJS)
	$(SS_LD) $^ -o $@ $(LDFLAGS)

clean:
	rm -rf $(OBJDIR) $(OBJS)
	rm -rf $(SS_BIN)

depclean:
	rm -rf $(SRCDIR)/*.d

