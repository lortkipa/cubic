cd engine
REM bear -- make -f linux.mk debug
make -f windows.mk debug
cd ..
cd testbed
REM bear -- make -f linux.mk debug
make -f windows.mk debug
make -f windows.mk run
cd ..
