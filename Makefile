#
# This is makefile for ssearcher
#
# Copyright (c) 2013 by Yang Hong <hy dot styx at gmail dot com>
#

# SUBDIR := snippets test
SUBDIR := 
OBJDIR := obj
DEPDIR := dep
UNAME_S := $(shell uname -s)

# test environment variables
TEST_HEADERS := -I. -Igtest/include
TEST_LD_PATH := -Lgtest/build -L/usr/lib
TEST_LDFLAGS := -lgtest_main -lgtest -pthread

SRCS :=
OBJS :=

TARGETS :=

.PHONY: default clean depclean test-build-env test

# Make sure that 'default' is the first target
default:

include sf.mk
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
	$(SF_CC) $(CFLAGS) -I. -MM $< -MT $(patsubst %.c,$(OBJDIR)/%.o, $<) > $@;

$(OBJDIR)/%.o: %.c
	@mkdir -p $(@D)
	$(SF_CC) $(CFLAGS) -I. -c $< -o $@

clean:
	rm -rf $(TARGETS) $(TEST_SUITE) *.dSYM
	rm -rf $(OBJDIR) $(OBJS)
	rm -rf gtest/build

depclean:
	rm -rf $(DEPDIR)/*.d $(DEPDIR)

test-build-env:
	@mkdir -p gtest/build
	@cd gtest/build; cmake ..; make

# $(TEST_SUITE): test-build-env wouldn't work because that
# will add `test-build-env' as a prerequisite to
# each of $(TEST_SUITE) and cause `$^' in other rules
# containing `test-build-env'
$(TEST_SUITE): | test-build-env

test: $(TEST_SUITE)
	@perl -e 'map { system("./$$_") == 0 or die "fail to run test $$_" } @ARGV' $(TEST_SUITE)
