# Makefile for testing program snippets

# SNPT_SRC := filemagic.c file-mt.c
OBJDIR := ../obj/snippets
# SNPT_SRC := test_pool.c
# SNPT_OBJ := $(SNPT_SRC:.c=.o)
SNPT_OBJ := $(OBJDIR)/snippets/test_pool.o
SNPT_BIN := test_pool

$(OBJDIR)/%.o: %.c
	@mkdir -p $(@D)
	cc -I../src -c $< -o $@

# FIXME: ../pool.o is the prerequisition of make ..
$(OBJDIR)/test_pool: $(OBJDIR)/test_pool.o $(OBJDIR)/../pool.o
	cc $^ -o $@

$(OBJDIR)/snippets/%: $(OBJDIR)/snippets/%.o
	cc -o $@ $^

all: (OBJDIR)/$(SNPT_BIN)

# filemagic: filemagic.o
# 	$(SS_CC) -o $@ $<

# file-mt: file-mt.o
# 	$(SS_CC) -o $@ $<
