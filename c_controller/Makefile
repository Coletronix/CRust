# Makefile for the Texas Instruments MSP432P401R LaunchPad Development Kit (MSP-EXP432P401R).
# Supports compilation, flashing, and debugging.
#
# Depends on msp432-gcc-support-files available at:
# 	https://software-dl.ti.com/msp430/msp430_public_sw/mcu/msp430/MSP432GCC/latest/index_FDS.html
#
# Depends on
# 	GCC
# 	OpenOCD
# 	GDB
# 
# Blake Batson - 1/18/2024
# Adapted by:
# Cole Johnson - 4/26/2024
.SILENT:
	


USRSRC = src/main.c
LABSRCS  = src/uart.c src/leds.c src/switches.c src/Timer32.c src/CortexM.c src/ADC14.c src/ServoMotor.c src/TimerA.c src/Motors.c src/ControlPins.c src/SysTickTimer.c src/slow.c src/fast.c src/pid.c src/oled.c src/i2c.c src/offsetCalc.c src/fastest.c src/fasterist.c src/driveBySerial.c

###############################################################################
# MSP432 GCC Support Files
###############################################################################
MSPDIR   = msp432-gcc-support-files
MSPINC   = $(MSPDIR)/include
CMSISINC = $(MSPDIR)/include/CMSIS
USRINC   = include
STARTUP  = $(MSPDIR)/src/startup_msp432p401r_gcc.c
SYSTEM   = $(MSPDIR)/src/system_msp432p401r.c
LINKERSCRIPT = $(MSPINC)/msp432p401r.lds

DEVICE = MSP432P401R

###############################################################################
# Compilation
###############################################################################
SRC = $(STARTUP) $(SYSTEM) $(USRSRC) $(LABSRCS)
OBJ = $(SRC:.c=.o)
ELF = firmware.elf
HEX = $(ELF:.elf=.hex)
MAPFILE = firmware.map

GCC_INSTALL_PATH = /usr/bin
GCC_TRIPLET = arm-none-eabi

CC = $(GCC_INSTALL_PATH)/$(GCC_TRIPLET)-gcc
CFLAGS  += -mcpu=cortex-m4 -march=armv7e-m -mfloat-abi=hard -mfpu=fpv4-sp-d16 -mthumb -D__$(DEVICE)__ -DTARGET_IS_MSP432P4XX -Dgcc -g -gstrict-dwarf -Wall -ffunction-sections -fdata-sections -MD -std=c99 -g -O0 -I$(MSPINC) -I$(CMSISINC) -I$(USRINC) -Wa,-adhlns="$@.lst"
LDFLAGS += -mcpu=cortex-m4 -march=armv7e-m -mfloat-abi=hard -mfpu=fpv4-sp-d16 -mthumb -D__$(DEVICE)__ -DTARGET_IS_MSP432P4XX -Dgcc -g -gstrict-dwarf -Wall -T$(LINKERSCRIPT) -l'c' -l'gcc' -l'nosys' -Wl,-Map=$(MAPFILE) -specs=nano.specs -specs=nosys.specs
OBJCPY = $(GCC_INSTALL_PATH)/$(GCC_TRIPLET)-objcopy

# GDB may or may not be prefixed with the triplet
GDB = $(GCC_INSTALL_PATH)/gdb-multiarch
#GDB = $(GCC_INSTALL_PATH)/gdb

all: $(HEX)

$(HEX): $(ELF)
	@echo "Generating: $@ ..."
	$(OBJCPY) -O ihex $< $@

# Link the firmware
$(ELF): $(OBJ)
	@echo "Linking: $@ ..."
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@ -lm

# Compile C files
.c.o:
	@echo "Compiling: $< ..."
	$(CC) $(CFLAGS) -c $< -o $@

###############################################################################
# Programming/Debugging
###############################################################################
OPENOCD = openocd
OCDSCRIPTS = /usr/share/openocd/scripts/board
OCDCFG = $(OCDSCRIPTS)/ti_msp432_launchpad.cfg

flash: $(HEX)
	openocd -f $(OCDCFG) -c "program $(HEX) verify reset exit"

debugserver: $(HEX)
	# openocd -f $(OCDCFG) -c ""
	openocd -f $(OCDCFG) -c "program $(HEX) verify reset; halt"

debug: $(ELF)
	$(GDB) -tui -ex "target extended-remote localhost:3333" -ex "break main" -ex "j main" $(ELF)

###############################################################################
# Clean
###############################################################################

clean:
	-rm -f $(HEX) $(ELF) $(OBJ) $(OBJ:.o=.d) $(addsuffix .lst,$(OBJ)) $(addsuffix .lst,$(ELF)) $(MAPFILE); find . -type f \( -name '*.o' -o -name '*.d' -o -name '*.lst' \) -exec rm -f {} +
