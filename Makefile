CC := cl65
CFLAGS := -t c64 -Iinclude

SOURCES := source
SOURCEFILES := $(wildcard $(SOURCES)/*.c)

default:	$(SOURCEFILES)
	$(CC) $(CFLAGS) -o rsbsPLUS-c64.prg -C c64.cfg $(SOURCEFILES)
