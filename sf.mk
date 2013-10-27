TARGETS += sf

SRCS +=	src/options.c	\
	src/main.c	\
	src/match.c	\
	src/thread.c	\
	src/util.c	\
	src/magic.c	\
	src/file.c	\
	src/buffer.c

magic.c: magic.c.tmpl
	@utils/gen_magic_header.pl

OBJS += $(patsubst %.c,%.o,$(subst src,obj/debug,$(SRCS)))

sf: $(OBJS)
	$(SF_LD) $^ -o sf $(LDFLAGS)
