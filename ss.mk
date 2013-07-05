TARGETS += ssearcher

SRCS +=	options.c \
	main.c \
	match.c \
	worker.c

OBJS += $(patsubst %.c,$(OBJDIR)/%.o,$(SRCS))

ssearcher: $(OBJS)
	$(SS_LD) $^ -o $@ $(LDFLAGS)

