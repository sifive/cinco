# README #

This repository allows you to program Freedom E300 boards using the Arduino IDE. Follow the instructions below to install the Board support package manually. Note that SiFive also supports downloading the tools in a precompiled state, or installing the Freedom E SDK seperately. Please see the Getting Started Guides for more information.

[Freedom E310 Arty Dev Kit Getting Started Guide](https://dev.sifive.com/develop/freedom-e310-arty-dev-kit-v1-0/)

[HiFive1 Getting Started Guide](https://dev.sifive.com/hifive1/)

# Setup #

## Install Arduino ##

Download and install Arduino IDE 1.6.12 tarball from the Arduino website. Unpack it and run their installation script as directed.

## Install this Repo ###

* clone this repo wherever you like. Assume that you set an environment variable CINCO to that location.

```
cd $CINCO
git clone http://github.com/sifive/cinco.git
```

* Create a simlink from your Arduino install location:

```
cd /opt/arduino-1.6.12/hardware/
ln -s $CINCO/hardware sifive
```

## Install RISC-V Tools and OpenOCD ##

```
cd $CINCO
git submodule update --init --recursive
cd hardware/freedom_e/freedom-e-sdk
make tools
```

Add the toolchain to your path:

```
export PATH=$CINCO/hardware/freedom_e/freedom-e-sdk/toolchain/bin:$PATH
```

## Select Your Board ##

Restart and launch the Arduino IDE.

Select the board (e.g. Freedom E300 Arty Dev Kit) on the Arduino Menu

Tools->Board->Freedom E 300 Dev Kit

## Select OpenOCD as the  Programmer ##

Tools->Programmer->OpenOCD

## Write & Upload Your Program ##

Select an example program and modify it as usual.

For example, use the 'Blink' example, which needs
no modifications.

Hit the "Verify" button to test the program compiles,
then "Upload" to program to the board. The green LED should blink.
