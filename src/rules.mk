TARGETS += ssearcher

SRCS +=	src/options.c \
	src/main.c \
	src/match.c \
	src/pool.c

OBJS += $(patsubst $(SRCDIR)/%.c,$(OBJDIR)/%.o,$(SRCS))

ssearcher: $(OBJS)
	$(SS_LD) $^ -o $@ $(LDFLAGS)

