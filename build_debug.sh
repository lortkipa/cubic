cd engine
make -f linux.mk debug
bear -- make -f linux.mk debug
cd ..

cd testbed
make -f linux.mk debug
bear -- make -f linux.mk debug
cd ..
