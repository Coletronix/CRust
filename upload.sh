#!/bin/bash

bin_name=rust_controller

cargo build -p $bin_name --target thumbv7em-none-eabihf

# check if last command successful and exit if not
if [ $? -ne 0 ]; then
    echo "Build failed"
    exit 1
fi

GCC_INSTALL_PATH=/usr/bin
GCC_TRIPLET=arm-none-eabi
OBJCPY=$GCC_INSTALL_PATH/$GCC_TRIPLET-objcopy
OPENOCD=openocd
OCDSCRIPTS=/usr/share/openocd/scripts/board
OCDCFG=$OCDSCRIPTS/ti_msp432_launchpad.cfg

BINPATH=target/thumbv7em-none-eabihf/debug
ELF=$BINPATH/$bin_name
HEX=$BINPATH/$bin_name.hex

$OBJCPY -O ihex $ELF $HEX

echo $OCDCFG
openocd -f $OCDCFG -c "program $HEX verify reset exit"