# MSP432-Linux

Example repo for building, flashing, and debugging the MSP432 LaunchPad board on Linux

## Usage

### Building

Edit the list of your source files in the Makefile and run `make`.

### Flashing

Just run `make flash` and OpenOCD will flash the board with the hex file.

### Debugging

Run `make debugserver` to start the OpenOCD debug server for GDB to connect to. You can run `make debug` to connect to the server easily. Alternatively, start GDB targeting the firmware ELF file and run the command `target extended-remote localhost:3333` to connect.

## Dependencies

### MSP432 GCC Support Files

The MSP432 GCC support files must be downloaded directly from TI [here](https://software-dl.ti.com/msp430/msp430_public_sw/mcu/msp430/MSP432GCC/latest/index_FDS.html).

### GCC for arm-none-eabi

The arm-none-eabi toolchain may be downloaded directly from arm [here](https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads). If your distribution provides it you should install it from your package manager instead.

### OpenOCD

OpenOCD is likely packaged in your package manager. If not there are other ways to obtain it, see the official website [here](https://openocd.org/pages/getting-openocd.html).
