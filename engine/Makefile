project = engine
compiler = clang
sources = $(shell find src -name *.c)
libraries = 
flags = -Isrc -Wall -Wextra 

build:
	echo building $(project)...
	$(compiler) $(sources) $(libraries) $(flags) -fPIC -shared -o ../bin/lib$(project).so
