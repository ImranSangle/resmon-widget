echo starting debugger...
cd build
make 
gdb ./resmon-widget -ex "set disassembly-flavor intel" -x ../gdbconfig.txt
