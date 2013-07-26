#
# This is makefile for ssearcher
#
# Copyright (c) 2013 by Yang Hong <hy dot styx at gmail dot com>
#

SUBDIR := snippets test
OBJDIR := obj
DEPDIR := dep
UNAME_S := $(shell uname -s)

SRCS :=
OBJS :=

TARGETS :=
TEST_SUITE :=

.PHONY: default clean depclean test

# Make sure that 'default' is the first target
default:

include ss.mk
include $(patsubst %, %/rules.mk, $(SUBDIR))
include common.mk

default: $(TARGETS)

# -include Makefile.dep
# dep: $(SRCS:.c=.d)
# 	@echo "Finish building dependencies" # just do something to make dependencies executed
# .PHONY: dep

# include automatically generated dependencies
include $(patsubst %.c, $(DEPDIR)/%.d,$(SRCS))

# Generage dependencies like:
# obj/main.o: src/main.c src/options.h src/debug.h
# Make sure the target of dependencies match
# binary files in `obj' directory
# -MT: specify the target file
# -MM: specify input file(s)
$(DEPDIR)/%.d: %.c
	@mkdir -p $(@D)
	$(SS_CC) $(CFLAGS) -I. -MM $< -MT $(patsubst %.c,$(OBJDIR)/%.o, $<) > $@;

$(OBJDIR)/%.o: %.c
	@mkdir -p $(@D)
	$(SS_CC) $(CFLAGS) -I. -c $< -o $@

clean:
	rm -rf $(TARGETS) $(TEST_SUITE)
	rm -rf $(OBJDIR) $(OBJS)
	rm -rf gtest/build

depclean:
	rm -rf $(DEPDIR)/*.d $(DEPDIR)

test-build-env:
	@mkdir -p gtest/build
	@cd gtest/build; cmake ..; make

# TEST_SUITE := $(patsubst %,%.t,$(TEST_BIN))

# test-all: $(TEST_SUITE)

# .PHONY: $(TEST_SUITE)

# $(TEST_SUITE): $(TEST_BIN)
# 	@./$(patsubst %.t,%,$@)

test:
	@perl -e 'map { system("./$$_") == 0 or die "fail to run test $$_" } @ARGV' $(TEST_SUITE)
