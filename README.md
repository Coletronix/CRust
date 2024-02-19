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



# Line Following Robot Car Project

## Project Idea
The project aims to develop a line-following robot car with controller implementation in both Rust and C. The controllers will interface with the simulation of the car model in Rust to track the actual car position with the predicted one in the model.

## Hardware (HW)
- **Microcontroller:** MSP432P401R MCU
  - **Architecture:** ARM Cortex-M4F
  - **Clock Speed:** Up to 48 MHz
  - **Flash Memory:** 256 KB
  - **RAM:** 64 KB
  - **Features:** Low power consumption, integrated peripherals for analog and digital I/O, UART, SPI, I2C interfaces, and support for real-time applications.

## Software (SW)
- Programming Languages: Rust, C
- Build System: Make

## Control Algorithms
There are three different algorithms for car control:
1. **PID (Proportional-Integral-Derivative):** A control loop feedback mechanism that calculates the error between a desired setpoint and a measured process variable, and applies a correction based on proportional, integral, and derivative terms.
2. **Pure Pursuit:** A path tracking algorithm that calculates the curvature required to follow a path by considering a lookahead point on the path.
3. **Stanley Controller:** A steering control algorithm that adjusts the steering angle based on the cross-track error (distance between the vehicle and the path) and the heading error (difference between the vehicle's orientation and the path's orientation).


For Pure Pursuit and Stanley Controller, there are two options for the line-following algorithm, which depend on the current location and model of the track. There is a consideration of using a particle filter for determining position.

## Simulation Requirements
The simulation requires high performance. The model of the track is stored in the MCU, potentially done at runtime and stored in Flash. The block dimensions are known, and approximately five tracks (with 50 blocks each) will be built to train the algorithm, check its efficiency, and select the best one.

## Viewer/Simulation Tasks
1. View particle filter for visual simulation and testing.
2. Once the physical car is built, the purpose of the viewer will be to track the car's physical location compared to the simulated one.

## Goals
1. Write documentation along the way of development.
2. Get the C controller working.
3. Ensure no compiler warnings.
4. For Rust implementation: avoid trait objects.