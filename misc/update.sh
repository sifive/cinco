#! /bin/bash
rm -rf mcs.old
mv mcs mcs.old
mkdir mcs
MCSDIR=rimas@gamma02:/home/rimas/guava/federation/builds/guava_arty_dontboot/mcs/
scp ${MCSDIR}*.mcs mcs
scp ${MCSDIR}*.prm mcs
scp ${MCSDIR}*.bit mcs
scp ${MCSDIR}obj/system.early.ltx mcs

vivado_lab -nojournal -mode batch -source prog_flash.tcl

