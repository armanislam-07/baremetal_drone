CC := arm-none-eabi-gcc
OBJCOPY := arm-none-eabi-objcopy
SIZE := arm-none-eabi-size
CFLAGS := -std=c11 -mcpu=cortex-m3 -mthumb -mfloat-abi=soft -Os -ffunction-sections -fdata-sections -Wall -Wextra -Werror
AUTOSTART ?= 0
CFLAGS += -DDRONE_AUTOSTART=$(AUTOSTART)
LDFLAGS := -mcpu=cortex-m3 -mthumb -mfloat-abi=soft -T STM32F103RBTX_FLASH.ld -Wl,--gc-sections -Wl,-Map=build/drone.map --specs=nano.specs --specs=nosys.specs
SOURCES := $(wildcard src/*.c src/drivers/*.c src/app/*.c)
OBJECTS := $(patsubst %.c,build/%.o,$(SOURCES)) build/startup/startup_stm32f103rbtx.o
all: build/drone.elf build/drone.bin
build/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@
build/startup/%.o: startup/%.s
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@
build/drone.elf: $(OBJECTS) STM32F103RBTX_FLASH.ld
	$(CC) $(OBJECTS) $(LDFLAGS) -lm -lc -o $@
	$(SIZE) $@
build/drone.bin: build/drone.elf
	$(OBJCOPY) -O binary $< $@
clean:
	rm -rf build
test:
	clang -std=c11 -Wall -Wextra -Werror tests/flight_test.c src/app/autolevel.c -lm -o /tmp/drone-flight-test
	/tmp/drone-flight-test
.PHONY: all clean test
