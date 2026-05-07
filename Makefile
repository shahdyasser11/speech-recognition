# ================================================================
#  Makefile — ATmega32A Speech Recognition Project
# ================================================================

# ── Chip settings ──────────────────────────────────────────────
MCU    = atmega32
F_CPU  = 11059200UL
TARGET = speech_recognition

# ── Toolchain ──────────────────────────────────────────────────
CC      = avr-gcc
OBJCOPY = avr-objcopy
AVRDUDE = avrdude

# ── Compiler flags ─────────────────────────────────────────────
#   -mmcu      : target chip
#   -DF_CPU    : pass F_CPU value into the code as a #define
#   -O2        : optimise (important on 8-bit MCU)
#   -Wall      : show all warnings
#   -Iinc      : look in the inc/ folder for header files
#   -lm        : link the math library (needed for sqrtf)
CFLAGS = -mmcu=$(MCU) -DF_CPU=$(F_CPU) -O2 -Wall -Iinc -lm -std=c99

# ── Source files ───────────────────────────────────────────────
SRC = src/main.c     \
      src/adc.c      \
      src/timer.c    \
      src/features.c \
	  src/uart.c\
	  src/lcd.c

# ================================================================
#  BUILD RULES
# ================================================================

# default rule — runs when you type:  make
all: $(TARGET).hex

# compile all .c files → one .elf file
$(TARGET).elf: $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET).elf

# convert .elf → .hex  (the format avrdude needs to flash)
$(TARGET).hex: $(TARGET).elf
	$(OBJCOPY) -O ihex $(TARGET).elf $(TARGET).hex

# ── flash to chip ─────────────────────────────────────────────
# runs when you type:  make flash
# change -c usbasp to match your programmer (usbasp / arduino / stk500)
flash: $(TARGET).hex
	$(AVRDUDE) -c usbasp -p m32 -U flash:w:$(TARGET).hex

# ── clean build files ─────────────────────────────────────────
# runs when you type:  make clean
clean:
	rm -f $(TARGET).elf $(TARGET).hex
