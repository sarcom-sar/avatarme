CC := gcc
CFLAGS := -Wall
LINKLIBS := /usr/local/lib/libcargs.a -lcrypto

all: avatarme

avatarme: avatarme.c identicon.c $(LINKLIBS)
	$(CC) $(CFLAGS) -o $@ $^

.PHONY: clean
clean: avatarme
	rm $^
