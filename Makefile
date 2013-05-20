#
# This is makefile for ssearcher
#
# Copyright (c) 2013 by Yang Hong, All Rights Reserved.

SRCDIR := src
OBJDIR := obj

SS_OBJS := $(OBJDIR)/options.o $(OBJDIR)/main.o

# Make sure that 'all' is the first target
all: $(OBJDIR)/ssearcher

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	@echo \  CC -O2 $<
	@mkdir -p $(@D)
	@$(CC) -O2 -c -o $@ $<

$(OBJDIR)/ssearcher: $(SS_OBJS)
	@echo \  CC -o $@
	@$(CC) -o $@ $^

clean:
	rm -rf $(OBJDIR) $(SS_OBJS)
