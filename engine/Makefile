PROJECT = engine
COMPILER = clang
SOURCES = $(shell find src -name *.c)
LIBRARIES = -lX11
FLAGS = -fPIC -shared -Isrc

build_release:
	$(COMPILER) $(SOURCES) $(LIBRARIES) $(FLAGS) -o ../bin/lib$(PROJECT).so

build_debug:
	$(COMPILER) $(SOURCES) $(LIBRARIES) $(FLAGS) -DDEBUG -Wall -Wextra -g -o ../bin/lib$(PROJECT).so
