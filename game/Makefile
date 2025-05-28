PROJECT = game
COMPILER = clang
SOURCES = $(shell find src -name *.c)
LIBRARIES = -L../bin -lengine
FLAGS = -Isrc -I../engine/src

build_release:
	$(COMPILER) $(SOURCES) $(LIBRARIES) $(FLAGS) -o ../bin/$(PROJECT)

build_debug:
	$(COMPILER) $(SOURCES) $(LIBRARIES) $(FLAGS) -DDEBUG -Wall -Wextra -g -o ../bin/$(PROJECT)

run:
	LD_LIBRARY_PATH=../bin ../bin/$(PROJECT)

run_debug:
	export LD_LIBRARY_PATH=../bin; gdb ../bin/$(PROJECT)

