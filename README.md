# README #

### Setup ###

* download and install Arduino IDE 1.6.12 tarball from the Arduino website. Unpack it in /opt and run their installation script.
* Check out this repo wherever you like, and then create a symlink:
```
cd /opt/arduino-1.6.12/hardware/sifive 
ln -s path-to-repo/hardware sifive
```
* Edit /opt/arduino-1.6.12/hardware/sifive/riscv/platform.txt

This is where the locations of tools, command line options, etc are defined.  They can be overridden by settings in boards.txt (for the case when you have same CPU but pins/programming/etc is different at the board level).

** set compiler.path to point to your install of riscv-tools, i.e.:

compiler.path=/home/rimas/install/riscv-tools-c173fd6/bin/

** change tools.lsketch.path to path-to-repo/misc

* Install OpenOCD from https://github.com/sifive/openocd.git
* Commented out one line to start a programming running easily, see misc/.diff to show the change.  
* Note that scripts are hard-coded assuming openod was installed at /opt/openocd.

* Plug in two usb cables to micro usb connector on arty board and mini usb connector on FT2232H minimodule board.

Make sure devices show up as /dev/ttyUSB0-3.  The ordering of these is nondeterministic based on when order that things get plugged in/etc but that only matters when you want to use serial console.  I've been using minicom:
```
minicom -D /dev/ttyUSB1 (or sometimes ttyUSB3)
``
when you connect to the right port, it will cause a reset of the CPU/FPGA.
