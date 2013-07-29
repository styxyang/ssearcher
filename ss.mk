TARGETS += ss

SRCS +=	ss_options.c	\
	ss_main.c	\
	ss_match.c	\
	ss_thread.c	\
	ss_trie.c	\
	ss_magic.c	\
	ss_file.c

ss_magic.c: ss_magic.c.tmpl
	@utils/gen_magic_header.pl

OBJS += $(patsubst %.c,$(OBJDIR)/%.o,$(SRCS))

ss: $(OBJS)
	$(SS_LD) $^ -o $@ $(LDFLAGS)

