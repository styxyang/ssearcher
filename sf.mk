TARGETS += sf

SRCS +=	options.c	\
	main.c		\
	match.c		\
	thread.c	\
	trie.c		\
	magic.c		\
	file.c		\
	buffer.c

magic.c: magic.c.tmpl
	@utils/gen_magic_header.pl

OBJS += $(patsubst %.c,$(OBJDIR)/%.o,$(SRCS))

sf: $(OBJS)
	$(SF_LD) $^ -o $@ $(LDFLAGS)

