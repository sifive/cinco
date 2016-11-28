# README #

### Setup ###

## Install Arduino ##

Download and install Arduino IDE 1.6.12 tarball from the Arduino website. Unpack it in /opt and run their installation script.

## Install this Repo ###
* clone this repo wherever you like, and then create a symlink:
```
cd /opt/arduino-1.6.12/hardware/
ln -s path-to-this-repo/hardware sifive
```

## Install RISC-V Tools and OpenOCD ##

```
cd path-to-this-repo
git submodule update --init --recursive
cd freedom-e-sdk
make tools
```

Add variables to your path:

```
export CINCO=path-to-this-repo
export PATH=$CINCO/freedom-e-sdk/toolchain/bin:$PATH
```

Make sure you have permissons to communicate with your USB
devices. Generally this means adding yourself to the 'dialout' or
'plugdev' group(s). 

## Connecting to the HiFive1 Board


* Make sure a jumper is installed for IOREF, to either 3.3V or 5V, depending
on the shield or other devices you want to communicate with.

* Plug in one  USB micro cables to the micro USB connector on the HiFive1
board.

* The device will show up as FTDI Dual RS232. Determine which
serial port to use, e.g. `/dev/ttyUSB0-3`.
The ordering of these is nondeterministic based on when order that things
get plugged in/etc but that only matters when you want to use serial console.  

```
screen /dev/ttyUSB1 <baud rate>
```

* You can optionally use an external 7-12V DC Power supply. The
power supply will switch from USB automatically.

## Connecting to the Freedom E300 Arty Dev Kit (with Olimex Debugger)

* Connect one USB cables to the micro USB connector on the Arty board,
and one USB cable to the Olimex. Follow the instructions at

https://dev.sifive.com/develop/freeom-e00-arty-dev-kit

to connect your Arty board to the Olimex debugger.

* Determine where the devices show up as `/dev/ttyUSB0-3`.
The ordering of these is nondeterministic based on when order that things
get plugged in/etc but that only matters when you want to use serial console.  

```
minicom -D /dev/ttyUSB1 (or sometimes ttyUSB3)
```

## Select Your Programmer ##

Select the board (e.g. Freedom E300 Arty Dev Kit) on the Arduino Menu

Tools->Board->Freedom E 300 Dev Kit

Tools->Programmer->openocd

## Write & Upload Your Program ##

Select an example program and modify it as usual.

For example, use the 'Blink' example, which needs
no modifications.

Hit the "Verify" button to test the program compiles,
then "Upload" to program to the board.