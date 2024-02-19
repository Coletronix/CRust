#!/bin/bash

bin_name=rust_controller

# run in seperate shell and ignore output
openocd &> /dev/null &
FIRST_PID=$!

# Function to kill the openodb process when exiting this script
cleanup() {
    echo "Killing openodb (PID $FIRST_PID)"
    kill $FIRST_PID
}

# Trap EXIT and call cleanup
trap cleanup EXIT

cargo build -p $bin_name --target thumbv7em-none-eabihf

# check if last command successful and exit if not
if [ $? -ne 0 ]; then
    echo "Build failed"
    exit 1
fi

gdb-multiarch -tui -q target/thumbv7em-none-eabihf/debug/$bin_name -x gdb_commands.gdb