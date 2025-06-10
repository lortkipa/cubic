COMPILER = clang
SOURCES = $(shell find src -name *.c)
LIBRARIES = -lengine
FLAGS = -Wall -Wextra -Isrc -I../engine/src -L../bin
RELEASE_FLAGS = -O3
DEBUG_FLAGS = -g -DDEBUG -O0

release:
	$(COMPILER) $(SOURCES) $(LIBRARIES) $(FLAGS) $(RELEASE_FLAGS) -o ../bin/testbed

debug:
	$(COMPILER) $(SOURCES) $(LIBRARIES) $(FLAGS) $(DEBUG_FLAGS) -o ../bin/testbed

run:
	LD_LIBRARY_PATH=../bin ./../bin/testbed

run_debug:
	LD_LIBRARY_PATH=../bin gdb ../bin/testbed
