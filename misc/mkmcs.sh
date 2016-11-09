#!/bin/sh
exec vivado -nojournal -mode batch -source mcs.tcl -tclargs "${1}"
