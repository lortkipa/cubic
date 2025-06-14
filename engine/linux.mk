COMPILER = clang
SOURCES = $(shell find src -name *.c)
LIBRARIES = -lX11 -lvulkan
FLAGS = -Wall -Wextra -fPIC -shared -Isrc
RELEASE_FLAGS = -O3
DEBUG_FLAGS = -g -DDEBUG -O0

release:
	$(COMPILER) $(SOURCES) $(LIBRARIES) $(FLAGS) $(RELEASE_FLAGS) -o ../bin/libengine.so

debug:
	$(COMPILER) $(SOURCES) $(LIBRARIES) $(FLAGS) $(DEBUG_FLAGS) -o ../bin/libengine.so
