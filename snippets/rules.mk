# Makefile for testing program snippets

SNIPPETS_BIN :=

##############################################
# test_worker
##############################################
test_worker_SRC := snippets/test_worker.c
test_worker_OBJ := $(patsubst %.c,$(OBJDIR)/%.o,$(test_worker_SRC))
test_worker_BIN := test_worker

# FIXME: ../worker.o is the prerequisition of make ..
$(test_worker_BIN): $(test_worker_OBJ) $(OBJDIR)/ss_thread.o
	$(SS_LD) -o $@ $^ $(LDFLAGS)

SRCS += $(test_worker_SRC)
TARGETS += $(test_worker_BIN)

##############################################
# test_pipe
##############################################
test_pipe_SRC := snippets/test_pipe.c
test_pipe_OBJ := $(patsubst %.c,$(OBJDIR)/%.o,$(test_pipe_SRC))
test_pipe_BIN := test_pipe

# FIXME: ../pipe.o is the prerequisition of make ..
$(test_pipe_BIN): $(test_pipe_OBJ)
	$(SS_LD) -o $@ $^ $(LDFLAGS)

SRCS += $(test_pipe_SRC)
TARGETS += $(test_pipe_BIN)
