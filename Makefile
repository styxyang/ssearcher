
SRCDIR := src
SRC := src
OBJDIR := obj

SS_OBJS := $(OBJDIR)/options.o $(OBJDIR)/main.o
OBJ := $(patsubst %.c,%.o,$(filter %.c,$(SRC)))

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	@echo \  CC -O2 $<
	@mkdir -p $(@D)
	@$(CC) -O2 -c -o $@ $<

default: $(SS_OBJS)
	@echo \  CC -o $@
	@$(CC) -o $@ $^

clean:
	rm -rf $(OBJDIR) $(SS_OBJS)
	rm default
