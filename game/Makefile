PROJECT = game
COMPILER = clang
SOURCES = $(shell find src -name *.c)
LIBRARIES = 
FLAGS = -Isrc

build_release:
	$(COMPILER) $(SOURCES) $(LIBRARIES) $(FLAGS) -o ../bin/$(PROJECT)

build_debug:
	$(COMPILER) $(SOURCES) $(LIBRARIES) $(FLAGS) -DDEBUG -Wall -Wextra -g -o ../bin/$(PROJECT)

run:
	../bin/$(PROJECT)

run_debug:
	gdb ../bin/$(PROJECT)
