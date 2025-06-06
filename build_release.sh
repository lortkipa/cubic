cd engine
make -f linux.mk release
bear -- make -f linux.mk release
cd ..

cd testbed
make -f linux.mk debug
bear -- make -f linux.mk release
cd ..
