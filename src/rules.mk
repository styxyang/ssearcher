SRCS +=	src/options.c \
	src/main.c \
	src/match.c \
	src/pool.c

OBJS += $(patsubst $(SRCDIR)/%.c,$(OBJDIR)/%.o,$(SRCS))

