COMPILER = clang
SOURCES = $(shell find src -name *.c)
LIBRARIES = 
FLAGS = -fPIC -shared -Isrc

release:
	$(COMPILER) $(SOURCES) $(LIBRARIES) $(FLAGS) -o ../bin/libengine.so

debug:
	$(COMPILER) $(SOURCES) $(LIBRARIES) $(FLAGS) -g -Wall -Wextra -DDEBUG -o ../bin/libengine.so
