# README #

### Setup ###

## Install Arduino ##

Download and install Arduino IDE 1.6.12 tarball from the Arduino website. Unpack it in /opt and run their installation script.

## Install RISC-V Tools ##

You will at least need the 32-bit riscv-gnu-toolchain and openocd. You could
install SiFive's Freedom E300 SDK to get all that you need.

## Install this Repo ###
* clone this repo wherever you like, and then create a symlink:
```
cd /opt/arduino-1.6.12/hardware/
ln -s path-to-this-repo/hardware sifive
```

* Add variables to your path:

```
export CINCO=path-to-this-repo
export PATH=path-to-riscv-toolschain-install:$PATH
export PATH=path-to-openocd-install:$PATH
```

## Connect to the Board ##

* Plug in two usb cables to micro usb connector on arty board and mini usb connector on FT2232H minimodule board.

* Make sure devices show up as /dev/ttyUSB0-3.  The ordering of these is nondeterministic based on when order that things get plugged in/etc but that only matters when you want to use serial console.  
```
minicom -D /dev/ttyUSB1 (or sometimes ttyUSB3)
```
when you connect to the right port, it will cause a reset of the CPU/FPGA.

## Select Your Programmer ##

Select the board (e.g. Freedom E300 Arty Dev Kit) on the Arduino Menu

Tools->Board->Freedom E 300 Dev Kit

## Write & Upload Your Program ##

Select an example program and modify it as usual.
So far, I'm testing the "Fade" example.

Arduino distinguishes between "Programming" and
"Uploading". By default, you just program the memory.
In our flow, it [will be] a menu item to select whether
it is flashed or loaded into memory. The difference is just
in how it is compiled, because the GDB loader does
the right thing based on the memory map.
