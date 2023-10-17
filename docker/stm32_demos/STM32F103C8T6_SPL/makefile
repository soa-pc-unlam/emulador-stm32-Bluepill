# DECLARATIONS

# Directory where QEMU_STM32 ARM executable is located - used for running
# program in emulator.
QEMU_ARM_DIR ?= ../qemu_stm32/arm-softmmu/

# OpenOCD interface file used for programming/debugging the micronctroller
OPENOCD_INTERFACE ?= interface/olimex-arm-usb-tiny-h.cfg

# Declare command line tools - assume these are in the path
CC	  = arm-none-eabi-gcc
LD	  = arm-none-eabi-ld
AS	  = arm-none-eabi-as
CP	  = arm-none-eabi-objcopy
OD	  = arm-none-eabi-objdump

# Declare command line flags
CORE_CFLAGS = -I./ -I$(CORE_SRC) -I$(DEVICE_SRC) -I$(STD_PERIPH)/inc -Icommon -fno-common -O0 -g -mcpu=cortex-m3 -mthumb 
CFLAGS  = $(CORE_CFLAGS) -c 
CFLAGS_LINK = -Wl,-Tmain.ld -nostartfiles $(CORE_CFLAGS)
ASFLAGS = -mcpu=cortex-m3 -mthumb -g
LDFLAGS = -Tmain.ld -nostartfiles
CPFLAGS = -Obinary
ODFLAGS	= -S

# Declare library source paths
SRC = ../
CORE_SRC = $(SRC)/libraries/CMSIS/CM3/CoreSupport
DEVICE_SRC = $(SRC)/libraries/CMSIS/CM3/DeviceSupport/ST/STM32F10x
STD_PERIPH = $(SRC)/libraries/STM32F10x_StdPeriph_Driver
STD_PERIPH_SRC = $(STD_PERIPH)/src

# List common and system library source files
# (i.e. for accessing STM32/Cortex M3 hardware) 
COMMON_FILES = $(CORE_SRC)/core_cm3.c
COMMON_FILES += $(DEVICE_SRC)/system_stm32f10x.c
COMMON_FILES += $(DEVICE_SRC)/startup/gcc_ride7/startup_stm32f10x_md.s
COMMON_FILES += common/stm32_f103.c
COMMON_FILES += common/myprintk.c
COMMON_FILES += $(STD_PERIPH_SRC)/stm32f10x_rcc.c
COMMON_FILES += $(STD_PERIPH_SRC)/stm32f10x_gpio.c
COMMON_FILES += $(STD_PERIPH_SRC)/stm32f10x_usart.c
COMMON_FILES += $(STD_PERIPH_SRC)/stm32f10x_exti.c
COMMON_FILES += $(STD_PERIPH_SRC)/stm32f10x_adc.c
COMMON_FILES += $(STD_PERIPH_SRC)/stm32f10x_tim.c
COMMON_FILES += $(STD_PERIPH_SRC)/stm32f10x_rtc.c
COMMON_FILES += $(STD_PERIPH_SRC)/stm32f10x_dac.c
COMMON_FILES += $(STD_PERIPH_SRC)/misc.c

# List all demos
DEMOS =  adc_single
DEMOS += blink_flash
DEMOS += blink_flash_asm
DEMOS += button
DEMOS += button_int
DEMOS += button_int_infinite
DEMOS += c_mem_model
DEMOS += freertos_singlethread
DEMOS += freertos_multithread
DEMOS += freertos_streambuffer
DEMOS += qemu_test
DEMOS += software_int
DEMOS += stkalign
DEMOS += systick
DEMOS += timer
DEMOS += printf_demo
DEMOS += uart_echo
DEMOS += uart_echo_int
DEMOS += uart_repeat_write
DEMOS += uart_repeat_write_int
DEMOS += rtc
DEMOS += dac
DEMOS += freertos_semaphore1 
DEMOS += freertos_cycletask

# List all demo folders
DEMO_FOLDERS = $(addprefix ,$(DEMOS))

# List FreeRTOS resources
FREE_RTOS_SRC = $(SRC)/libraries/FreeRTOS
FREE_RTOS_SRC_FILES = $(FREE_RTOS_SRC)/croutine.c $(FREE_RTOS_SRC)/list.c $(FREE_RTOS_SRC)/queue.c $(FREE_RTOS_SRC)/tasks.c $(FREE_RTOS_SRC)/croutine.c $(FREE_RTOS_SRC)/stream_buffer.c $(FREE_RTOS_SRC)/portable/GCC/ARM_CM3/port.c
FREE_RTOS_INC = $(FREE_RTOS_SRC)/include/
FREE_RTOS_PORT_INC = $(FREE_RTOS_SRC)/portable/GCC/ARM_CM3/

# List path to demo build output files
OUTPUT_FILES = $(addsuffix /main,$(DEMO_FOLDERS))
ELF_FILES	= $(addsuffix .elf,$(OUTPUT_FILES))
LIST_FILES	= $(addsuffix .list,$(OUTPUT_FILES))
BIN_FILES	= $(addsuffix .bin,$(OUTPUT_FILES))

# Declare target names for each demo
ALL_TARGETS = $(addsuffix _ALL,$(DEMOS))
PROG_TARGETS = $(addsuffix _PROG,$(DEMOS))

QEMU_RUN_TARGETS = $(addsuffix _QEMURUN,$(DEMOS))
QEMU_RUN_PTY_TARGETS = $(addsuffix _QEMURUN_PTY,$(DEMOS))
QEMU_RUN_TEL_TARGETS = $(addsuffix _QEMURUN_TEL,$(DEMOS))
QEMU_DBG_TARGETS = $(addsuffix _QEMUDBG,$(DEMOS))
QEMU_DBG_PTY_TARGETS = $(addsuffix _QEMUDBG_PTY,$(DEMOS))
QEMU_DBG_TEL_TARGETS = $(addsuffix _QEMUDBG_TEL,$(DEMOS))



# TARGETS - See README for descriptions of the targets

# Generic targets
.PHONY: clean $(ALL_TARGETS) $(PROG_TARGETS) openocd_dbg

all: $(ALL_TARGETS)

clean:
	find . -type f -name "*.o" -exec rm {} \;
	find . -type f -name "*.elf" -exec rm {} \;
	find . -type f -name "*.bin" -exec rm {} \;
	find . -type f -name "*.list" -exec rm {} \; 

# Compile targets (builds all output files)
$(ALL_TARGETS): %_ALL : %/main.elf %/main.bin %/main.list

# Targets to program a microntroller using OpenOCD
$(PROG_TARGETS): %_PROG : %_ALL
	-killall -q openocd
	openocd -f $(OPENOCD_INTERFACE) -f openocd/openocd_stm32_p103.cfg -c "program_flash $*/main.bin"

# Target to launch OpenOCD - by default, OpenOCD creates a GDB server at port 3333.
DBG:
	-killall -q openocd
	openocd -f $(OPENOCD_INTERFACE) -f openocd/openocd_stm32_p103.cfg -c "init_stm32"
	
# QEMU run targets
$(QEMU_RUN_TARGETS): %_QEMURUN : %_ALL
	-killall -q qemu-system-arm
	$(QEMU_ARM_DIR)qemu-system-arm -M stm32-f103c8 -kernel $*/main.bin

$(QEMU_RUN_PTY_TARGETS): %_QEMURUN_PTY : %_ALL
	-killall -q qemu-system-arm
	$(QEMU_ARM_DIR)qemu-system-arm -M stm32-f103c8 -kernel $*/main.bin -serial pty
	
$(QEMU_RUN_TEL_TARGETS): %_QEMURUN_TEL : %_ALL
	-killall -q qemu-system-arm
	$(QEMU_ARM_DIR)qemu-system-arm -M stm32-f103c8 -kernel $*/main.bin -serial tcp::7777,server

# QEMU debug targets
$(QEMU_DBG_TARGETS): %_QEMUDBG : %_ALL
	-killall -q qemu-system-arm
	$(QEMU_ARM_DIR)qemu-system-arm -M stm32-f103c8 -gdb tcp::3333 -S -kernel $*/main.bin

$(QEMU_DBG_PTY_TARGETS): %_QEMUDBG_PTY : %_ALL
	-killall -q qemu-system-arm
	$(QEMU_ARM_DIR)qemu-system-arm -M stm32-f103c8 -gdb tcp::3333 -S -kernel $*/main.bin -serial pty
	
$(QEMU_DBG_TEL_TARGETS): %_QEMUDBG_TEL : %_ALL
	-killall -q qemu-system-arm
	$(QEMU_ARM_DIR)qemu-system-arm -M stm32-f103c8 -gdb tcp::3333 -S -kernel $*/main.bin -serial tcp::7777,server
	
# Note: Use this command to run QEMU in low-level debug mode:
#    qemu-system-arm -cpu cortex-m3 -M stm32-f103c8 -nographic -singlestep -kernel main.bin -d in_asm,out_asm,exec,cpu,int,op,op_opt

# Compile targets to build individual files
$(LIST_FILES): %.list : %.elf
	$(OD) $(ODFLAGS) $< > $@

$(BIN_FILES): %.bin : %.elf
	$(CP) $(CPFLAGS) $< $@

# Targets to build individual demos
blink_flash/main.elf: blink_flash/main.c
blink_flash/main.elf: $(STD_PERIPH_SRC)/stm32f10x_rcc.c
blink_flash/main.elf: $(STD_PERIPH_SRC)/stm32f10x_gpio.c
blink_flash/main.elf: $(COMMON_FILES)
	$(CC) $(CFLAGS_LINK) -Iblink_flash/ -o $@ $^


blink_flash_asm/main.elf: blink_flash_asm/main.o
	$(LD) $(LDFLAGS) -nostartfiles -o $@ $<

blink_flash_asm/main.o: blink_flash_asm/main.s
	 $(AS) $(ASFLAGS) -o $@ $<


adc_single/main.elf: adc_single/main.c
adc_single/main.elf: $(COMMON_FILES)
	$(CC) $(CFLAGS_LINK) -Iadc_single/ -o $@ $^


button/main.elf: button/main.c
button/main.elf: $(COMMON_FILES)
	$(CC) $(CFLAGS_LINK) -Ibutton/ -o $@ $^


button_int/main.elf: button_int/main.c
button_int/main.elf: $(COMMON_FILES)
	$(CC) $(CFLAGS_LINK) -Ibutton_int/ -o $@ $^


button_int_infinite/main.elf: button_int/main.c
button_int_infinite/main.elf: $(COMMON_FILES)
	$(CC) -DDO_NOT_CLEAR_IT_PENDING_FLAG $(CFLAGS_LINK) -Ibutton_int/ -o $@ $^


c_mem_model/main.o: c_mem_model/main.c
	$(CC) $(CFLAGS_LINK) -Ic_mem_model/ -c -o $@ $^

c_mem_model/main.elf: c_mem_model/main.o
c_mem_model/main.elf: $(COMMON_FILES)
	$(CC) $(CFLAGS_LINK) -Ic_mem_model/ -o $@ $^


freertos_singlethread/main.elf: freertos_singlethread/main.c
freertos_singlethread/main.elf: $(COMMON_FILES)
freertos_singlethread/main.elf: $(FREE_RTOS_SRC_FILES)
freertos_singlethread/main.elf: $(FREE_RTOS_SRC)/portable/MemMang/heap_1.c
	$(CC) $(CFLAGS_LINK) -Ifreertos_singlethread/ -I$(FREE_RTOS_INC) -I$(FREE_RTOS_PORT_INC) -o $@ $^


freertos_multithread/main.elf: freertos_multithread/main.c
freertos_multithread/main.elf: $(COMMON_FILES)
freertos_multithread/main.elf: $(FREE_RTOS_SRC_FILES)
freertos_multithread/main.elf: $(FREE_RTOS_SRC)/portable/MemMang/heap_1.c
	$(CC) $(CFLAGS_LINK) -Ifreertos_multithread/ -I$(FREE_RTOS_INC) -I$(FREE_RTOS_PORT_INC) -o $@ $^

freertos_streambuffer/main.elf: freertos_streambuffer/main.c
freertos_streambuffer/main.elf: $(COMMON_FILES)
freertos_streambuffer/main.elf: $(FREE_RTOS_SRC_FILES)
freertos_streambuffer/main.elf: $(FREE_RTOS_SRC)/portable/MemMang/heap_1.c
	$(CC) $(CFLAGS_LINK) -Ifreertos_streambuffer/ -I$(FREE_RTOS_INC) -I$(FREE_RTOS_PORT_INC) -o $@ $^

qemu_test/main.elf: qemu_test/main.c
qemu_test/main.elf: $(COMMON_FILES)
	$(CC) $(CFLAGS_LINK) -Iqemu_test/ -o $@ $^


software_int/main.elf: software_int/main.c
software_int/main.elf: $(COMMON_FILES)
	$(CC) $(CFLAGS_LINK) -Isoftware_int/ -o $@ $^


stkalign/main.elf: stkalign/main.c
stkalign/main.elf: $(COMMON_FILES)
	$(CC) $(CFLAGS_LINK) -Istkalign/ -o $@ $^


systick/main.elf: systick/main.c
systick/main.elf: $(COMMON_FILES)
	$(CC) $(CFLAGS_LINK) -Isystick/ -o $@ $^


timer/main.elf: timer/main.c
timer/main.elf: $(COMMON_FILES)
	$(CC) $(CFLAGS_LINK) -Itimer/ -o $@ $^


uart_echo/main.elf: uart_echo/main.c
uart_echo/main.elf: $(COMMON_FILES)
	$(CC) $(CFLAGS_LINK) -Iuart_echo/ -o $@ $^

printf_demo/main.elf: printf_demo/main.c
printf_demo/main.elf: $(COMMON_FILES)
	$(CC) $(CFLAGS_LINK) -Iprintf_demo/ -o $@ $^

uart_repeat_write/main.elf: uart_repeat_write/main.c
uart_repeat_write/main.elf: $(COMMON_FILES)
	$(CC) $(CFLAGS_LINK) -Iuart_repeat_write/ -o $@ $^


uart_echo_int/main.elf: uart_echo_int/main.c
uart_echo_int/main.elf: $(COMMON_FILES)
	$(CC) $(CFLAGS_LINK) -Iuart_echo_int/ -o $@ $^

uart_repeat_write_int/main.elf: uart_repeat_write_int/main.c
uart_repeat_write_int/main.elf: $(COMMON_FILES)
	$(CC) $(CFLAGS_LINK) -Iuart_repeat_write_int/ -o $@ $^

dac/main.elf: dac/main.c
dac/main.elf: $(COMMON_FILES)
	$(CC) $(CFLAGS_LINK) -Idac/ -o $@ $^

rtc/main.elf: rtc/main.c
rtc/main.elf: $(COMMON_FILES)
	$(CC) $(CFLAGS_LINK) -Irtc/ -o $@ $^

freertos_semaphore1/main.elf: freertos_semaphore1/main.c
freertos_semaphore1/main.elf: $(COMMON_FILES)
freertos_semaphore1/main.elf: $(FREE_RTOS_SRC_FILES)
freertos_semaphore1/main.elf: $(FREE_RTOS_SRC)/portable/MemMang/heap_1.c
	$(CC) $(CFLAGS_LINK) -Ifreertos_semaphore1/ -I$(FREE_RTOS_INC) -I$(FREE_RTOS_PORT_INC) -o $@ $^

freertos_cycletask/main.elf: freertos_cycletask/main.c
freertos_cycletask/main.elf: $(COMMON_FILES)
freertos_cycletask/main.elf: $(FREE_RTOS_SRC_FILES)
freertos_cycletask/main.elf: $(FREE_RTOS_SRC)/portable/MemMang/heap_1.c
	$(CC) $(CFLAGS_LINK) -Ifreertos_cycletask/ -I$(FREE_RTOS_INC) -I$(FREE_RTOS_PORT_INC) -o $@ $^
