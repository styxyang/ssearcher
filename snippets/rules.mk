# Makefile for testing program snippets

# SNPT_SRC := filemagic.c file-mt.c
# OBJDIR := ../obj/snippets

# SNPT_SRC := test_pool.c
# SNPT_OBJ := $(SNPT_SRC:.c=.o)
SNPT_SRCS := snippets/test_pool.c

SNPT_OBJS += $(patsubst %,$(OBJDIR)/%,$(SNPT_SRCS:.c=.d))
SNPT_BIN := obj/snippets/test_pool
TARGETS += $(SNPT_BIN)

# test:
# 	echo $(SNPT_OBJS)

$(OBJDIR)/snippets/%.o: snippets/%.c
	@mkdir -p $(@D)
	$(SS_CC) $(CFLAGS) -Isrc -c -o $@ $<


# FIXME: ../pool.o is the prerequisition of make ..
$(OBJDIR)/snippets/test_pool: $(OBJDIR)/snippets/test_pool.o $(OBJDIR)/pool.o
	$(SS_LD) -o $@ $^ $(LDFLAGS)

# filemagic: filemagic.o
# 	$(SS_CC) -o $@ $<

# file-mt: file-mt.o
# 	$(SS_CC) -o $@ $<

