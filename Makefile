# ================================================================
#  Makefile — ATmega32A Speech Recognition Project
# ================================================================

# ── Chip settings ──────────────────────────────────────────────
MCU    = atmega32
F_CPU  = 11059200UL
TARGET = speech_recognition

# ── Toolchain ──────────────────────────────────────────────────
CC       = avr-gcc
OBJCOPY  = avr-objcopy
AVRDUDE  = avrdude

# ── Compiler flags ─────────────────────────────────────────────
#   -mmcu      : target chip
#   -DF_CPU    : pass F_CPU value into code
#   -O2        : optimization
#   -Wall      : enable warnings
#   -Iinc      : include directory
#   -std=c99   : C99 standard
CFLAGS = -mmcu=$(MCU) \
          -DF_CPU=$(F_CPU) \
          -O2 \
          -Wall \
          -Iinc \
          -std=c99

# ── Linker flags ───────────────────────────────────────────────
#   -lm              : math library
#   -lprintf_flt     : enable float printf
#   -u,vfprintf      : force float formatter
LDFLAGS = -Wl,-u,vfprintf \
           -lprintf_flt \
           -lm

# ── Source files ───────────────────────────────────────────────
SRC = src/main.c \
      src/adc.c \
      src/timer.c \
      src/features.c \
      src/uart.c \
      src/lcd.c

# ================================================================
#  BUILD RULES
# ================================================================

# default rule
all: $(TARGET).hex

# compile and link
$(TARGET).elf: $(SRC)
	$(CC) $(CFLAGS) $(SRC) $(LDFLAGS) -o $(TARGET).elf

# generate hex file
$(TARGET).hex: $(TARGET).elf
	$(OBJCOPY) -O ihex $(TARGET).elf $(TARGET).hex

# ── Flash to chip ──────────────────────────────────────────────
flash: $(TARGET).hex
	$(AVRDUDE) -c usbasp -p m32 -U flash:w:$(TARGET).hex

# ── Clean build files ──────────────────────────────────────────
clean:
	rm -f $(TARGET).elf $(TARGET).hex