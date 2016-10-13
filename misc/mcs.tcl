lassign $argv bin
set name [file rootname [file tail $bin]]
write_cfgmem -format mcs -interface spix4 -size 16 -loaddata "up 0x0 ${bin}" -file "${name}.mcs" -force
exit
