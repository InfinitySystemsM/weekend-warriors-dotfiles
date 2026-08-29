CC ?= gcc
CFLAGS ?= -Wall -Wextra -O3 -march=native
LDFLAGS ?= -lsndfile -lpulse-simple -lpulse -lpthread -lm
TARGET = sampledeck
PREFIX ?= $(HOME)/.local/bin

all: $(TARGET)

$(TARGET): sampledeck.c
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

install: $(TARGET)
	mkdir -p $(PREFIX)
	install -m 755 $(TARGET) $(PREFIX)/$(TARGET)

clean:
	rm -f $(TARGET)

.PHONY: all install clean
