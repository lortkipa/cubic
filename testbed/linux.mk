COMPILER = clang
SOURCES = $(shell find src -name *.c)
LIBRARIES = -lengine
FLAGS = -Isrc -I../engine/src -L../bin

release:
	$(COMPILER) $(SOURCES) $(LIBRARIES) $(FLAGS) -o ../bin/testbed

debug:
	$(COMPILER) $(SOURCES) $(LIBRARIES) $(FLAGS) -g -Wall -Wextra  -o ../bin/testbed
