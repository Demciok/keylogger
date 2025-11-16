# ---------- ZMIENNE KOMPILACJI ----------
CC = gcc
TARGET = key
SOURCE = key.c

# Używamy CFLAGS do flag kompilatora
# -D_DEFAULT_SOURCE jest kluczowe dla funkcji daemon()
CFLAGS = -Wall -Wextra -g -std=c99 -D_DEFAULT_SOURCE

# Używamy LDFLAGS do innych opcji linkera (często puste)
LDFLAGS = 

# Używamy LDLIBS do bibliotek
# Wymaga linkowania z libevdev i biblioteką czasu realnego (rt) dla niektórych systemów
# choć libevdev sama w sobie powinna działać
LDLIBS = -levdev

# Użycie pkg-config do automatycznego pobrania flag nagłówków dla libevdev
# Dodajemy te flagi do CFLAGS
export CFLAGS += $(shell pkg-config --cflags libevdev)
export LDLIBS += $(shell pkg-config --libs libevdev)

# ---------- CELE MAKE ----------
.PHONY: all clean

# Cel domyślny: buduje plik wykonywalny "key"
all: $(TARGET)

$(TARGET): $(SOURCE)
        # Komenda make automatycznie używa CFLAGS przy kompilacji
        $(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS) $(LDLIBS)

# Cel 'clean': usuwa skompilowany program
clean:
        rm -f $(TARGET)
