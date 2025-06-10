COMPILER = clang
SOURCES = $(wildcard src/**/*.c)
LIBRARIES = 
FLAGS = -Wall -Wextra -shared -Isrc
RELEASE_FLAGS = -O3
DEBUG_FLAGS = -g -DDEBUG -O0

release:
	$(COMPILER) $(SOURCES) $(LIBRARIES) $(FLAGS) $(RELEASE_FLAGS) -o ../bin/engine.dll

debug:
	$(COMPILER) $(SOURCES) $(LIBRARIES) $(FLAGS) $(DEBUG_FLAGS) -o ../bin/engine.dll
