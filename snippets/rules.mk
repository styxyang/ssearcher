# Makefile for testing program snippets

SNIPPETS_BIN :=

##############################################
# test_pool
##############################################
test_pool_SRC := snippets/test_pool.c
test_pool_OBJ := $(patsubst %.c,$(OBJDIR)/%.o,$(test_pool_SRC))
test_pool_BIN := test_pool

# FIXME: ../pool.o is the prerequisition of make ..
$(test_pool_BIN): $(test_pool_OBJ) $(OBJDIR)/pool.o
	$(SS_LD) -o $@ $^ $(LDFLAGS)

SRCS += $(test_pool_SRC)
TARGETS += $(test_pool_BIN)

##############################################
#
##############################################
