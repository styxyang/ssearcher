# Makefile for testing program snippets

SNIPPETS_BIN :=

##############################################
# test_worker
##############################################
test_worker_SRC := snippets/test_worker.c
test_worker_OBJ := $(patsubst %.c,$(OBJDIR)/%.o,$(test_worker_SRC))
test_worker_BIN := test_worker

# FIXME: ../worker.o is the prerequisition of make ..
$(test_worker_BIN): $(test_worker_OBJ) $(OBJDIR)/worker.o
	$(SS_LD) -o $@ $^ $(LDFLAGS)

SRCS += $(test_worker_SRC)
TARGETS += $(test_worker_BIN)

##############################################
#
##############################################
