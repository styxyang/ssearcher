SS_CC=$(QUIET_CC)$(CC)
SS_LD=$(QUIET_LINK)$(CC)
CFLAGS += -O0 -g

# -pthread is not needed on Darwin
ifneq ($(UNAME_S),Darwin)
LDFLAGS += -pthread
endif


# Borrowed from redis Makefile
# to make output colourful ^_^
CCCOLOR="\033[34m"
LINKCOLOR="\033[34;1m"
SRCCOLOR="\033[33m"
BINCOLOR="\033[37;1m"
MAKECOLOR="\033[32;1m"
ENDCOLOR="\033[0m"

ifndef V
QUIET_CC = @printf '    %b %b\n' $(CCCOLOR)CC$(ENDCOLOR) $(SRCCOLOR)$@$(ENDCOLOR) 1>&2;
QUIET_LINK = @printf '    %b %b\n' $(LINKCOLOR)LINK$(ENDCOLOR) $(BINCOLOR)$@$(ENDCOLOR) 1>&2;
QUIET_INSTALL = @printf '    %b %b\n' $(LINKCOLOR)INSTALL$(ENDCOLOR) $(BINCOLOR)$@$(ENDCOLOR) 1>&2;
endif
