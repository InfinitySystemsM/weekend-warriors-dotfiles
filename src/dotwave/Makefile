CC ?= gcc
CFLAGS ?= -Wall -Wextra -O3 -march=native
LDFLAGS ?= -lpulse-simple -lpulse -lm
TARGET = dotwave
PREFIX ?= $(HOME)/.local/bin

all: $(TARGET)

$(TARGET): dotwave.c
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

install: $(TARGET)
	mkdir -p $(PREFIX)
	install -m 755 $(TARGET) $(PREFIX)/$(TARGET)

clean:
	rm -f $(TARGET)

.PHONY: all install clean
