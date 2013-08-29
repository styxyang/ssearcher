# Makefile for testing program snippets

SNIPPETS_BIN :=

##############################################
# test_pipe
##############################################
test_pipe_SRC := snippets/test_pipe.c
test_pipe_OBJ := $(patsubst %.c,$(OBJDIR)/%.o,$(test_pipe_SRC))
test_pipe_BIN := test_pipe

# FIXME: ../pipe.o is the prerequisition of make ..
$(test_pipe_BIN): $(test_pipe_OBJ)
	$(SF_LD) -o $@ $^ $(LDFLAGS)

SRCS += $(test_pipe_SRC)
TARGETS += $(test_pipe_BIN)
