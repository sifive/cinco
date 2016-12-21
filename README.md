# README #

This repository allows you to program Freedom E300 boards using the Arduino IDE.
You can install this repository in two ways:

* Using the Arduino Boards Manager (Currently only supported for Linux)
* Manually compiling the tools (Works for Linux and Mac OSX).

Follow the instructions below to install the Board support package.

Please see the Getting Started Guides for more information on how to install and use the tools.

[Freedom E310 Arty Dev Kit Getting Started Guide](https://dev.sifive.com/develop/freedom-e310-arty-dev-kit-v1-0/)

[HiFive1 Getting Started Guide](https://dev.sifive.com/hifive1/)

# Setup #

## Install Arduino ##

Download and install Arduino IDE 1.6.12 tarball from the Arduino website. Unpack it and run their installation script as directed.

## Installing Through the Arduino IDE ##

Add the [http://static.dev.sifive.com/bsp/arduino/package_sifive_index.json](http://static.dev.sifive.com/bsp/arduino/package_sifive_index.json) to the Additional Board URLs.

Use the Board Manager to search for and install the "SiFive" boards.

## Install this Repo Manually ##

1. Clone this Repository

  Clone this repo wherever you like. Assume that you set an environment variable CINCO to that location.

  ```
  cd $CINCO
  git clone --recursive http://github.com/sifive/cinco.git
  ```

2. Create a simlink from your Arduino install location:

  ```
  cd /opt/arduino-1.6.12/hardware/
  ln -s $CINCO/hardware sifive
  ```
3. Install RISC-V Tools and OpenOCD

  If you have previously installed the Freedom E SDK, you do not need
  to do this step.

  Follow the instructions in the [https://github.com/sifive/freedom-e-sdk/blob/master/README.md](Freedom E SDK README) to install the SDK.

4. Add the toolchain to your path

  ```
  export PATH=$CINCO/hardware/freedom_e/freedom-e-sdk/toolchain/bin:$PATH
  ```

If you installed the Freedom E SDK some other way, use that installation
location instead.

# Select Your Board #

Restart and launch the Arduino IDE.

Select the board (e.g. Freedom E300 Arty Dev Kit) on the Arduino Menu

Tools->Board->Freedom E 300 Dev Kit

# Select Your Toolchain #

If you installed the tools using the Arduino Package Manager,
select `Tools -> Tool Install Location -> Default`.

If you compiled the Freedom E SDK manually,
select `Tools -> Tool Install Location -> Manual`.

# Select OpenOCD as the  Programmer #

If you installed the tools using the Arduino Package Manager,
select `Tools->Programmer->SiFive OpenOCD`

If you installed the tools manually, select
`Tools->Programmer-> Manual SiFive OpenOCD`

# Write & Upload Your Program #

Select an example program and modify it as usual.

For example, use the 'File->Examples->Basics->Blink' example, which needs
no modifications.

Hit the "Verify" button to test the program compiles,
then "Upload" to program to your board. The green LED should blink.
