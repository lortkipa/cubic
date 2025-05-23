project = testbed
compiler = clang
sources = $(shell find src -name *.c)
libraries = -L../bin -lengine
flags = -Isrc -I../engine/src

build:
	echo building $(project)...
	$(compiler) $(sources) $(libraries) $(flags) -o ../bin/$(project)

run:
	LD_LIBRARY_PATH=../bin/ ../bin/$(project)
