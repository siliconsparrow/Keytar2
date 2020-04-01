# ############################################################################
# ##
# ## Makefile for Keytar2 main software running on an STM32F746NGH6 
# ##
# ##   by Adam Pierce <adam@siliconsparrow.com>
# ##   created 20-Mar-2020
# ##
# ############################################################################

TARGET = Keytar2

SDK_PATH = STM32Cube_FW_F7_V1.16.0
DEBUGPATH = Debug
RELEASEPATH = Release
LINKER_SCRIPT_RELEASE = STM32F746NGHx_FLASH.ld
LINKER_SCRIPT_DEBUG = STM32F746NGHx_FLASH.ld

TOOLPATH = /opt/gcc-arm-none-eabi-9-2019-q4-major/bin

DEFINES  = -DSTM32F746xx

# List of all source files to compile.
SOURCES = \
	startup_stm32f746xx.s \
	system_stm32f7xx.c \
	stm32f7xx_ll_sdmmc.c \
	stm32f7xx_ll_usb.c \
	stm32f7xx_hal.c \
	stm32f7xx_hal_cortex.c \
	stm32f7xx_hal_gpio.c \
	stm32f7xx_hal_pwr_ex.c \
	stm32f7xx_hal_rcc.c \
	stm32f7xx_hal_rcc_ex.c \
	stm32f7xx_hal_i2c.c \
	stm32f7xx_hal_dma.c \
	stm32f7xx_hal_dma2d.c \
	stm32f7xx_hal_sd.c \
	stm32f7xx_hal_pcd.c \
	stm32f7xx_hal_pcd_ex.c \
	stm32f7xx_hal_sai.c \
	stm32f7xx_ll_fmc.c \
	stm32f7xx_hal_sdram.c \
	stm32f7xx_hal_ltdc.c \
	stm32f7xx_it.c \
	stm32746g_discovery_sd.c \
	platform.cpp \
	usbd_conf.c \
	usbd_core.c \
	usbd_desc.c \
	usbd_ctlreq.c \
	usbd_ioreq.c \
	usbd_msc.c \
	usbd_msc_bot.c \
	usbd_msc_scsi.c \
	usbd_msc_data.c \
	usbd_storage.c \
	ft5336.c \
	wm8994.c \
	stm32746g_discovery.c \
	stm32746g_discovery_sdram.c \
	stm32746g_discovery_lcd.c \
	stm32746g_discovery_ts.c \
	stm32746g_discovery_audio.c \
	Audio.cpp \
	Gui.cpp \
	crash.cpp \
	syscalls.cpp \
	tinycpp.cpp \
	main.cpp

#	sd_diskio.c \
	ff_gen_drv.c \
	diskio.c \
	ff.c \
	syscall.c \
	unicode.c \

	
#	$(wildcard *.c) \
#	$(wildcard *.cpp) \


# Directories in which we might find sources and dependencies.
VPATH = . \
	$(SDK_PATH)/Drivers/STM32F7xx_HAL_Driver/Src \
	$(SDK_PATH)/Drivers/BSP/STM32746G-Discovery \
	$(SDK_PATH)/Drivers/BSP/Components/ft5336 \
	$(SDK_PATH)/Drivers/BSP/Components/wm8994 \
	$(SDK_PATH)/Middlewares/Third_Party/FatFs/src \
	$(SDK_PATH)/Middlewares/Third_Party/FatFs/src/option \
	$(SDK_PATH)/Middlewares/ST/STM32_USB_Device_Library/Core/Src \
	$(SDK_PATH)/Middlewares/ST/STM32_USB_Device_Library/Class/MSC/Src
	
# Include paths
INCLUDES = -I. \
	-I$(SDK_PATH)/Drivers/CMSIS/Device/ST/STM32F7xx/Include \
	-I$(SDK_PATH)/Drivers/CMSIS/Include \
	-I$(SDK_PATH)/Drivers/STM32F7xx_HAL_Driver/Inc \
	-I$(SDK_PATH)/Drivers/BSP/STM32746G-Discovery \
	-I$(SDK_PATH)/Drivers/BSP/Components/ft5336 \
	-I$(SDK_PATH)/Middlewares/Third_Party/FatFs/src \
	-I$(SDK_PATH)/Middlewares/ST/STM32_USB_Device_Library/Core/Inc \
	-I$(SDK_PATH)/Middlewares/ST/STM32_USB_Device_Library/Class/MSC/Inc
	

# Additional libraries.
LIBS =

# Compiler flags for both C and C++.
#COMMONFLAGS = -fmessage-length=0 -specs=nosys.specs -mthumb -mcpu=cortex-m7 -fshort-wchar -fomit-frame-pointer -ffunction-sections -fdata-sections
COMMONFLAGS = -fmessage-length=0 -specs=nano.specs -mthumb -mcpu=cortex-m7 -fshort-wchar -fomit-frame-pointer -ffunction-sections -fdata-sections


# Debug and optimization flags.
#DEBUGFLAGS = -O3 -g0
DEBUGFLAGS = -D__DEBUG -g -O0
RELEASEFLAGS = -O3 -g0

# Additional compiler flags for C
CCONLYFLAGS = -std=gnu99

# Additional compiler flags for C++.
CXXONLYFLAGS = -fno-exceptions -fno-rtti

# Tools we are using. Make sure the system path allows access to all of these.
BUILDPREFIX = $(TOOLPATH)/arm-none-eabi-
AS      = $(BUILDPREFIX)gcc
CC      = $(BUILDPREFIX)gcc
CXX     = $(BUILDPREFIX)g++
LD      = $(BUILDPREFIX)g++
OBJCOPY = $(BUILDPREFIX)objcopy
SIZE    = $(BUILDPREFIX)size
MKDIR   = mkdir
RMDIR   = rm -Rf

# Build rules.
CFLAGS = $(COMMONFLAGS) $(CCONLYFLAGS) $(DEFINES) $(INCLUDES)
CXXFLAGS = $(COMMONFLAGS) $(CXXONLYFLAGS) $(DEFINES) $(INCLUDES)
ASFLAGS = $(COMMONFLAGS)
LDFLAGS_DEBUG = $(COMMONFLAGS) -Wl,--no-wchar-size-warning -Wl,--gc-sections -Xlinker -T$(LINKER_SCRIPT_DEBUG)
LDFLAGS_RELEASE = $(COMMONFLAGS) -Wl,--no-wchar-size-warning -Wl,--gc-sections -Xlinker -T$(LINKER_SCRIPT_RELEASE) 
OBJS = $(patsubst %.cpp,%.o,$(patsubst %.c,%.o,$(patsubst %.s,%.o,$(notdir $(SOURCES)))))

# ###################################
# Build targets

release: $(RELEASEPATH) $(RELEASEPATH)/$(TARGET).hex

debug: $(DEBUGPATH) $(DEBUGPATH)/$(TARGET).hex

# Delete working files and objects for both debug and release.
clean:
	$(RM)    $(RELEASEPATH)/$(TARGET).hex
	$(RM)    $(RELEASEPATH)/$(TARGET).elf
	$(RM)    $(RELEASEPATH)/$(TARGET).map
	$(RM)    $(addprefix $(RELEASEPATH)/,$(OBJS))
	$(RMDIR) $(RELEASEPATH)
	$(RM)    $(DEBUGPATH)/$(TARGET).hex
	$(RM)    $(DEBUGPATH)/$(TARGET).elf
	$(RM)    $(DEBUGPATH)/$(TARGET).map
	$(RM)    $(addprefix $(DEBUGPATH)/,$(OBJS))
	$(RMDIR) $(DEBUGPATH)


# ###################################
# RELEASE CONFIGURATION

# Create directory for object and binary files.
$(RELEASEPATH):
	$(MKDIR) $(RELEASEPATH)

$(RELEASEPATH)/%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(RELEASEFLAGS) -c -o $(RELEASEPATH)/$(@F) $<

$(RELEASEPATH)/%.o: %.c
	$(CC) $(CFLAGS) $(RELEASEFLAGS) -c -o $(RELEASEPATH)/$(@F) $<
	
$(RELEASEPATH)/%.o: %.s
	$(AS) $(ASFLAGS) -c -o $(RELEASEPATH)/$(@F) $<

# Linker.
$(RELEASEPATH)/$(TARGET).elf: $(addprefix $(RELEASEPATH)/,$(OBJS)) $(LINKER_SCRIPT)
	$(LD) $(LDFLAGS_RELEASE) -o $(RELEASEPATH)/$(TARGET).elf $(addprefix $(RELEASEPATH)/,$(OBJS)) $(LIBS) -Xlinker -Map=$(RELEASEPATH)/$(TARGET).map
	@# Print the size of the binary image.
	$(SIZE) $(RELEASEPATH)/$(TARGET).elf

# Create a HEX file for firmware flashing.
$(RELEASEPATH)/$(TARGET).hex: $(RELEASEPATH)/$(TARGET).elf
	$(OBJCOPY) -O ihex $(RELEASEPATH)/$(TARGET).elf $(RELEASEPATH)/$(TARGET).hex


# ###################################
# DEBUG CONFIGURATION

$(DEBUGPATH)/%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(DEBUGFLAGS) -c -o $(DEBUGPATH)/$(@F) $<

$(DEBUGPATH)/%.o: %.c
	$(CC) $(CFLAGS) $(DEBUGFLAGS) -c -o $(DEBUGPATH)/$(@F) $<
	
$(DEBUGPATH)/%.o: %.s
	$(AS) $(ASFLAGS) -c -o $(DEBUGPATH)/$(@F) $<

# Linker.
$(DEBUGPATH)/$(TARGET).elf: $(addprefix $(DEBUGPATH)/,$(OBJS)) $(LINKER_SCRIPT)
	$(LD) $(LDFLAGS_DEBUG) -o $(DEBUGPATH)/$(TARGET).elf $(addprefix $(DEBUGPATH)/,$(OBJS)) $(LIBS) -Xlinker -Map=$(DEBUGPATH)/$(TARGET).map
	@# Print the size of the binary image.
	$(SIZE) $(DEBUGPATH)/$(TARGET).elf

# Create a HEX file for firmware flashing.
$(DEBUGPATH)/$(TARGET).hex: $(DEBUGPATH)/$(TARGET).elf
	$(OBJCOPY) -O ihex $(DEBUGPATH)/$(TARGET).elf $(DEBUGPATH)/$(TARGET).hex

# Create directory for object and binary files.
$(DEBUGPATH):
	$(MKDIR) $(DEBUGPATH)