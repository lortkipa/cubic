cd engine
bear -- make -f linux.mk release
make -f linux.mk release
cd ..
cd testbed
bear -- make -f linux.mk release
make -f linux.mk release
make -f linux.mk run
cd ..
