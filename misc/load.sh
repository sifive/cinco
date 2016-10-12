#! /bin/bash

#echo "reset init" | nc 127.0.0.1 4444
echo "load_image /tmp/test.elf" | nc 127.0.0.1 4444
#echo "resume 0x80000000" | nc 127.0.0.1 4444

riscv32-unknown-elf-gdb -x ./gdb.command /tmp/test.elf
