#
# Makefile for msp430
#
# 'make' builds everything
# 'make clean' deletes everything except source files and Makefile
# You need to set TARGET, MCU and SOURCES for your project.
# TARGET is the name of the executable file to be produced 
# $(TARGET).elf $(TARGET).hex and $(TARGET).txt nad $(TARGET).map are all generated.
# The TXT file is used for BSL loading, the ELF can be used for JTAG use
# 
TARGET     = main
#MCU        = msp430g2452
MCU        = msp430g2553
#MCU        = msp430g2211
# List all the source files here
# eg if you have a source file foo.c then list it here
SOURCES = main.c ssd1306.c utilityfn.c ter_ssd1306.c rtc.c
# Include are located in the Include directory
INCLUDES = -I .
# Add or subtract whatever MSPGCC flags you want. There are plenty more
#######################################################################################
CFLAGS   = -std=c99 -pedantic -D__LINUX_MSP430_GCC__ -mmcu=$(MCU) -g -Os -Wall -Wunused $(INCLUDES)  
ASFLAGS  = -mmcu=$(MCU) -x assembler-with-cpp -Wa,-gstabs
LDFLAGS  = -mmcu=$(MCU) -Wl,-Map=$(TARGET).map
########################################################################################
TOOLPRFX = msp430-elf-
CC       = $(TOOLPRFX)gcc
LD       = $(TOOLPRFX)ld
AR       = $(TOOLPRFX)ar
AS       = $(TOOLPRFX)gcc
GASP     = $(TOOLPRFX)gasp
NM       = $(TOOLPRFX)nm
OBJCOPY  = $(TOOLPRFX)objcopy
RANLIB   = $(TOOLPRFX)ranlib
STRIP    = $(TOOLPRFX)strip
SIZE     = $(TOOLPRFX)size
READELF  = $(TOOLPRFX)readelf
MAKETXT  = srec_cat
CP       = cp -p
RM       = rm -f
MV       = mv
########################################################################################
# the file which will include dependencies
DEPEND = $(SOURCES:.c=.d)
# all the object files
OBJECTS = $(SOURCES:.c=.o)
# add $(TARGET).txt to the end of the below line and include the rule later on.
all: $(TARGET).elf $(TARGET).hex 
$(TARGET).elf: $(OBJECTS)
	echo "Linking $@"
	$(CC) $(OBJECTS) $(LDFLAGS) $(LIBS) -o $@
	echo
	echo ">>>> Size of Firmware <<<<"
	$(SIZE) $(TARGET).elf
	echo
%.hex: %.elf
	$(OBJCOPY) -O ihex $< $@
#%.ihex: %.hex
#	$(MAKETXT) -O $@ -TITXT $< -I
#	unix2dos $(TARGET).txt
#  The above line is required for the DOS based TI BSL tool to be able to read the txt file generated from linux/unix systems.
%.o: %.c
	echo "Compiling $<"
	$(CC) -c $(CFLAGS) -o $@ $<
# rule for making assembler source listing, to see the code
%.lst: %.c
	$(CC) -c $(ASFLAGS) -Wa,-anlhd $< > $@
# include the dependencies unless we're going to clean, then forget about them.
ifneq ($(MAKECMDGOALS), clean)
-include $(DEPEND)
endif
# dependencies file
# includes also considered, since some of these are our own
# (otherwise use -MM instead of -M)
%.d: %.c
	echo "Generating dependencies $@ from $<"
	$(CC) -M ${CFLAGS} $< >$@
.SILENT:
.PHONY:	clean
clean:
	-$(RM) $(OBJECTS)
	-$(RM) $(SOURCES:.c=.lst)
	-$(RM) *.map
	-$(RM) *.elf
	-$(RM) *.hex
	-$(RM) $(DEPEND)


