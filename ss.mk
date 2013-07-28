TARGETS += ssearcher

SRCS +=	ss_options.c	\
	ss_main.c	\
	ss_match.c	\
	ss_thread.c	\
	ss_trie.c	\
	ss_magic.c

OBJS += $(patsubst %.c,$(OBJDIR)/%.o,$(SRCS))

ssearcher: $(OBJS)
	$(SS_LD) $^ -o $@ $(LDFLAGS)

