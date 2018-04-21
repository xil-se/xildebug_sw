# XilDebug

[<img src="https://travis-ci.org/xil-se/xildebug_sw.svg?branch=master">](https://travis-ci.org/xil-se/xildebug_sw)

**XilDebug** is a piece of hardware that can act as a CMSIS-DAP compliant debugger, UART bridge and power profiler all in one package. This repository contains the firmware.

# Building

Make sure you have a recent gcc-arm-none-eabi toolchain. 7.2.1 is known to work.

`make` in the root builds the firmware.

# Debugging/flashing

Install a recent version of `openocd`.

Connect a debugger (e.g. an STLink or a CMSIS-DAP compliant debugger) to the SWD pins.

`make daplink` and `make stlink` starts `openocd` with the appropriate flags to debug a XilDebug device.

`make flash` flashes the firmware to the device.

# Usage

TODO

# Contributing

TBD. If you want to get involved feel free to post an issue.

# License

TBD

