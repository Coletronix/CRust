# The CRust Car Project!

This repository contains the code for our team's IDE car race project. It contains 3 sub-projects:

- simulation_viewer: A fancy gui that allows us to view what the racing algorithms are doing
- c_controller: Implementation of racing algorithms in C
- rust_controller: Implementation of racing algorithms in Rust

# Dependencies

Most of the dependencies will be automatically downloaded and built, however there are a few that need to be installed manually

### MSP432 GCC Support Files

The MSP432 GCC support files must be downloaded directly from TI [here](https://software-dl.ti.com/msp430/msp430_public_sw/mcu/msp430/MSP432GCC/latest/index_FDS.html).

### GCC for arm-none-eabi

The arm-none-eabi toolchain may be downloaded directly from arm [here](https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads). If your distribution provides it you should install it from your package manager instead.

### OpenOCD

OpenOCD is likely packaged in your package manager. If not there are other ways to obtain it, see the official website [here](https://openocd.org/pages/getting-openocd.html).

# How to Run

Running either `./run c` or `./run rust` will build and upload the C and Rust implementations of the racing algorithm respectively if the board is connected. 

Running `./run sim` will build and launch the simulation viewer, which will attempt to connect to the car via USB
