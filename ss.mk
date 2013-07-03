TARGETS += ssearcher

SRCS +=	options.c \
	main.c \
	match.c \
	pool.c

OBJS += $(patsubst %.c,$(OBJDIR)/%.o,$(SRCS))

ssearcher: $(OBJS)
	$(SS_LD) $^ -o $@ $(LDFLAGS)

