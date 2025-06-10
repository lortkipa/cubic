COMPILER = clang
SOURCES = $(wildcard src/*.c)
LIBRARIES = -lengine
FLAGS = -Wall -Wextra -Isrc -I../engine/src -L../bin
RELEASE_FLAGS = -O3
DEBUG_FLAGS = -g -DDEBUG -O0

release:
	$(COMPILER) $(SOURCES) $(LIBRARIES) $(FLAGS) $(RELEASE_FLAGS) -o ../bin/testbed.exe

debug:
	$(COMPILER) $(SOURCES) $(LIBRARIES) $(FLAGS) $(DEBUG_FLAGS) -o ../bin/testbed.exe

run:
	./../bin/testbed.exe

run_debug:
	gdb ../bin/testbed.exe
