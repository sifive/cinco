# README #

This repository allows you to program Freedom E300 boards using the Arduino IDE.
You can install this repository in two ways:

* Using the Arduino Boards Manager to download precompiled binaries.
* Manually compiling the tools.

Follow the instructions below to install the Board support package.

Please see the Getting Started Guides for more information on how to install and use the tools.

[Freedom E310 Arty Dev Kit Getting Started Guide](https://dev.sifive.com/develop/freedom-e310-arty-dev-kit-v1-0/)

[HiFive1 Getting Started Guide](https://dev.sifive.com/hifive1/)

# Setup #

## For Windows istallation ##

Follow the insructions in the [https://github.com/westerndigitalcorporation/CincoWinPkg/blob/master/README.md](https://github.com/westerndigitalcorporation/CincoWinPkg/blob/master/README.md)
## For Linux and MacOS installation ##
### Install Arduino ###

Download and install Arduino IDE 1.6.12 or later tarball from the Arduino website. Unpack it and run their installation script as directed.

### Install the SiFive Boards ###

Use one of the following methods:

#### Option 1: Installing Through the Arduino IDE ####

Add the [https://raw.githubusercontent.com/westerndigitalcorporation/CincoWinPkg/master/package_sifive_index.json](https://raw.githubusercontent.com/westerndigitalcorporation/CincoWinPkg/master/package_sifive_index.json)
to the Additional Boards Manager URLs in the `Preferences->Settings`.

#### Option 2: Install this Repo Manually ####

** This is generally not supported. ** You can use this technique if you want to work on the code in this repository.

1. Clone this Repository

  Clone this repo wherever you like. Assume that you set an environment variable CINCO to that location.

  ```
  cd $CINCO
  git clone --recursive http://github.com/sifive/cinco.git
  ```

2. Create a simlink from your Arduino install location. 

  ```
  cd /opt/arduino-1.6.12/hardware/
  ln -s $CINCO/hardware sifive
  ```

3. Install RISC-V Tools and OpenOCD

  If you have previously installed the Freedom E SDK, you do not need
  to do this step.

  Follow the instructions in the [Freedom E SDK README](https://github.com/sifive/freedom-e-sdk/blob/master/README.md) to install the SDK. You can use the version included in this repository or download and install it separately.

4. Add the toolchain to your path

  ```
  export PATH=$CINCO/hardware/freedom_e/freedom-e-sdk/toolchain/bin:$PATH
  ```

If you installed the Freedom E SDK some other way, use that installation
location instead.

### Select Your board: ###

Select the board in the Arduino Menu `Tools->Board->HiFive1`

### Select Your Toolchain ###

If you installed the tools using the Arduino Package Manager,
select `Tools -> Tool Install Location -> Default`.

If you compiled the Freedom E SDK manually, select
`Tools -> Tool Install Location -> Manual`.

### Select OpenOCD as the Programmer ###

If you installed the tools using the Arduino Package Manager, select
`Tools->Programmer->SiFive OpenOCD`

If you installed the tools manually, select
`Tools->Programmer-> Manual SiFive OpenOCD`

### Write & Upload Your Program ###

Select an example program and modify it as usual.

For example, use the 'File->Examples->Basics->Blink' example, which needs
no modifications.

Hit the "Verify" button to test the program compiles,
then "Upload" to program to your board. The green LED should blink.
