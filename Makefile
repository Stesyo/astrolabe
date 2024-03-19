TARGET = astrolabe
CC = clang
FLAGS = -Wall --pedantic

SRCDIR = src
BINDIR = bin

SOURCES  := $(wildcard $(SRCDIR)/*.c)
INCLUDES := $(wildcard $(SRCDIR)/*.h)
OBJECTS  := $(SOURCES:$(SRCDIR)/%.c=$(BINDIR)/%.o)


$(BINDIR)/$(TARGET): $(OBJECTS)
	@$(CC) $(OBJECTS) $(FLAGS) -o $@

$(BINDIR)/%.o : $(SRCDIR)/%.c
	@mkdir -p $(BINDIR)
	@$(CC) $(FLAGS) -c $< -o $@

.PHONY: clean
clean:
	@rm -f temp/*
	@rm -fd temp
	@rm -f $(BINDIR)/*
