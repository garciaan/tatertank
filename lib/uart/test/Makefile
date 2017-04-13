#PRG            = mega128_gcc_test_user
PRG = main
# Change "demo" above to the name of your project with NO extension.


# You should not have to change anything below here, but go ahead and
# try to understand what is going on.

OBJ            	= $(PRG).o 
OBJ			   	+= ../../lib/I2C-master-lib/i2c_master.o
OBJ			   	+= ../../lib/adc/adc.o
OBJ			   	+= ../../lib/lcd/lcd.o
OBJ				+= ../../lib/uart/uart.o
OBJ			   	+= ../../lib/motors/motors.o
OBJ 		   	+= ../../lib/magnometer/magnometer.o
OBJ 		   	+= ../../lib/lasersensor/lasersensor.o 
OBJ				+= ../../lib/pressuresensor/pressuresensor.o

MCU_TARGET     = atmega128
OPTIMIZE       = -O2    # options are 1, 2, 3, s

DEFS           =
LIBS           = -lm
CC             = avr-gcc

# Override is only needed by avr-lib build system.

override CFLAGS        = -g -Wall $(OPTIMIZE) -mmcu=$(MCU_TARGET) $(DEFS)
override LDFLAGS       = -Wl,-Map,$(PRG).map

OBJCOPY        = avr-objcopy
OBJDUMP        = avr-objdump

all: $(PRG).elf lst text eeprom size

$(PRG).elf: $(OBJ)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)
clean: 
	rm -rf *.o $(PRG).elf *.eps *.png *.pdf *.bak 
	rm -rf *.lst *.map $(EXTRA_CLEAN_FILES) *~

program: $(PRG).hex size
	avrdude -c osuisp2 -p atmega128 -P usb -e -U flash:w:$(PRG).hex
	avr-size --mcu=$(MCU_TARGET) -C $(PRG).elf

size: $(PRG).elf
	avr-size --mcu=$(MCU_TARGET) -C $(PRG).elf
lst:  $(PRG).lst

%.lst: %.elf
	$(OBJDUMP) -h -S $< > $@

# Rules for building the .text rom images

text: hex bin srec

hex:  $(PRG).hex
bin:  $(PRG).bin
srec: $(PRG).srec

%.hex: %.elf
	$(OBJCOPY) -j .text -j .data -O ihex $< $@

%.srec: %.elf
	$(OBJCOPY) -j .text -j .data -O srec $< $@

%.bin: %.elf
	$(OBJCOPY) -j .text -j .data -O binary $< $@

# Rules for building the .eeprom rom images

eeprom: ehex ebin esrec

ehex:  $(PRG)_eeprom.hex
ebin:  $(PRG)_eeprom.bin
esrec: $(PRG)_eeprom.srec

%_eeprom.hex: %.elf
	$(OBJCOPY) -j .eeprom --change-section-lma .eeprom=0 -O ihex $< $@

%_eeprom.srec: %.elf
	$(OBJCOPY) -j .eeprom --change-section-lma .eeprom=0 -O srec $< $@

%_eeprom.bin: %.elf
	$(OBJCOPY) -j .eeprom --change-section-lma .eeprom=0 -O binary $< $@

# Every thing below here is used by avr-libc's build system and can be ignored
# by the casual user.

FIG2DEV                 = fig2dev
EXTRA_CLEAN_FILES       = *.hex *.bin *.srec

dox: eps png pdf

eps: $(PRG).eps
png: $(PRG).png
pdf: $(PRG).pdf

%.eps: %.fig
	$(FIG2DEV) -L eps $< $@

%.pdf: %.fig
	$(FIG2DEV) -L pdf $< $@

%.png: %.fig
	$(FIG2DEV) -L png $< $@
