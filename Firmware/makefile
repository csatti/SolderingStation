## Platform
ARM_COMPILE			= 	arm-atollic-eabi
CPU 				= 	-mcpu=cortex-m4
FPU 				= 	-mfpu=fpv4-sp-d16
FLOAT-ABI 			= 	-mfloat-abi=hard
PLATFORM			?=	ARM
# link script
LDSCRIPT 			= 	stm32f4_flash.ld

## Optimization
DEBUG				?= 	0
OPTDBG				= 	-O0 -g
OPTREL				= 	-O2

## Board
# High speed external crystal
C_DEF				= 	HSE_VALUE=8000000

## Defines
C_DEF 				+= 	USE_FULL_LL_DRIVER \
						STM32F405xx \
						STM32F4XX \
						STM32F40XX
AS_DEF 				=	

## Build path
BUILDDIR 			= 	build
OBJDIR				= 	obj

## TARGET
TARGET  			?= 	SolderingStation

## Paths
LIB 				= 	Libraries
SRC 				= 	src
PLT 				= 	src/platform

## Git
VERSION_SHORT		=	$(shell git describe --tags --abbrev=0)
VERSION				=	$(shell git describe --tags --long)
COMMITDATETIME		=	$(shell git log -1 --date=format:'%Y%m%d-%H%M' --format=%cd)
BRANCH				=	$(shell git rev-parse --abbrev-ref HEAD)

## Platform independent includes
## Includes (-I added later)
# AS includes
AS_INC		 		=	

# C includes
C_INC	 			= 	$(LIB)/FreeRTOS/Source/include \
						$(LIB)/liblzg/src/include \
						$(SRC)/FreeRTOS \
						$(SRC)/UI \
						$(SRC)/UI/resources \
						$(SRC)/UI/widgets \
						$(SRC)/UI/localization \
						$(PLT)/Common \
						$(PLTDIR)/board \
						$(PLTDIR)/control \
						$(PLTDIR) \
						$(SRC) \
						$(PLTDIR)/ugfx \

## Sources
# AS sources					
ASM_SOURCES 		=  	

# C sources
C_SOURCES 			+= 	$(wildcard $(LIB)/FreeRTOS/Source/*.c)
C_SOURCES 			+= 	$(wildcard $(LIB)/liblzg/src/lib/*.c)
C_SOURCES 			+= 	$(wildcard $(SRC)/FreeRTOS/*.c)
C_SOURCES 			+= 	$(wildcard $(SRC)/*.c)
C_SOURCES 			+= 	$(wildcard $(SRC)/UI/*.c)
C_SOURCES 			+= 	$(wildcard $(SRC)/UI/widgets/*.c)
C_SOURCES			+= 	$(SRC)/UI/resources/image_raw.c
C_SOURCES			+= 	$(SRC)/UI/resources/image_comp.c
C_SOURCES 			+= 	$(wildcard $(SRC)/UI/localization/*.c)
C_SOURCES 			+= 	$(wildcard $(PLT)/Common/*.c)
C_SOURCES 			+= 	$(wildcard $(PLTDIR)/board/*.c)
C_SOURCES 			+= 	$(wildcard $(PLTDIR)/control/*.c)
C_SOURCES 			+= 	$(wildcard $(PLTDIR)/*.c)

## Platform dependent includes
ifeq ($(PLATFORM), Win32)
	# Win32
	PLTDIR 			=	$(PLT)/Win32
	BUILDDIRP 		=	$(BUILDDIR)/Win32
	CROSS_COMPILE 	=	
	MCU 			= 
	
	# C includes
	C_INC	 		+= 	$(LIB)/FreeRTOS/Source/portable/MSVC-MingW \
						$(PLTDIR)/FreeRTOS \
	
	# C sources
	C_SOURCES 		+= 	$(wildcard $(LIB)/FreeRTOS/Source/portable/MSVC-MingW/*.c) 
	
	# Linked libraries
	LIBS			= 
	LDLIBS			=   $(LIB)/Win32/libwinmm.a \
						$(LIB)/Win32/libgdi32.a \
	# Build command
	pall 			= 	path $(VER) $(EXE)
else
	# ARM
	PLTDIR 			= 	$(PLT)/ARM
	BUILDDIRP 		= 	$(BUILDDIR)/ARM
	CROSS_COMPILE 	=	$(ARM_COMPILE)-
	OPTDBG			+= 	-gdwarf
	## Microcontroller
	MCU 			= 	$(CPU) -mthumb $(FPU) $(FLOAT-ABI) -specs=nano.specs -fstack-usage
	## ARM MATH
	C_DEF 			+=	ARM_MATH_CM4
	
	# C includes
	C_INC	 		+= 	$(LIB)/FreeRTOS/Source/portable/GCC/ARM_CM4F \
						$(LIB)/STM32F4xx_HAL_Driver/Inc \
						$(LIB)/Device/ST/STM32F4xx/Include \
						$(LIB)/CMSIS/Include \
						$(PLTDIR)/ugfx/Buttons \
						$(PLTDIR)/ugfx/ILI9341 \
						$(PLTDIR)/ugfx/Rotary \
	#					$(PLTDIR)/ugfx/SDIO \
	
	# AS includes
	AS_INC		 	+=	$(PLTDIR)
	
	# AS sources					
	ASM_SOURCES 	+=  $(PLTDIR)/startup_stm32f407xx.s
	
	# C sources
	C_SOURCES 		+= 	$(wildcard $(LIB)/STM32F4xx_HAL_Driver/Src/stm32f4xx_ll*.c)
	C_SOURCES 		+= 	$(wildcard $(LIB)/FreeRTOS/Source/portable/GCC/ARM_CM4F/*.c)
	C_SOURCES 		+= 	$(wildcard $(PLTDIR)/ugfx/Buttons/*.c)
	C_SOURCES 		+= 	$(wildcard $(PLTDIR)/ugfx/ILI9341/*.c)
	C_SOURCES 		+= 	$(wildcard $(PLTDIR)/ugfx/Rotary/*.c)
	#C_SOURCES 		+= 	$(wildcard $(PLTDIR)/ugfx/SDIO/*.c)
	
	# Linked libraries
	LIBS 			= 	-lnosys
	LDLIBS			=   $(LIB)/CMSIS/Lib/libarm_cortexM4lf_math.a
	
	# Build command
	pall 			= path $(VER) $(ELF) $(HEX) #$(BIN)
endif

## External libraries (with own make)
# uGFX
GFXLIB = $(LIB)/ugfx
include $(GFXLIB)/gfx.mk
ifeq ($(PLATFORM), Win32)
include $(GFXLIB)/drivers/multiple/Win32/driver.mk
endif
C_SOURCES += $(GFXSRC)
C_INC += $(GFXINC)

## Add -D to defines
C_DEFINES =$(foreach d, $(C_DEF), -D$d)
AS_DEFINES =$(foreach d, $(AS_DEF), -D$d)

## Add -I to includes
C_INCLUDES =$(foreach d, $(C_INC), -I$d)
AS_INCLUDES =$(foreach d, $(AS_INC), -I$d)

## Cross-compilation commands 
CC = $(CROSS_COMPILE)gcc
AS = $(CROSS_COMPILE)gcc
LD = $(CROSS_COMPILE)gcc
AR = $(CROSS_COMPILE)ar
OC = $(CROSS_COMPILE)objcopy
SZ = $(CROSS_COMPILE)size

## Optimization
ifeq ($(DEBUG), 1)
OPT = $(OPTDBG)
BUILD_DIR = $(BUILDDIRP)/Debug
else
OPT = $(OPTREL)
BUILD_DIR = $(BUILDDIRP)/Release
endif

## Compile flags
CFLAGS = -c $(MCU) $(C_DEFINES) $(OPT) $(C_INCLUDES)
CFLAGS += -Wall -fdata-sections -ffunction-sections -DDEBUG=${DEBUG}

# Generate dependency information
CFLAGS += -MMD -MP -MF"$(@:%.o=%.d)" 

ASFLAGS = -c $(MCU) $(AS_DEFINES) $(OPT) $(AS_INCLUDES)
ASFLAGS += -Wall -fdata-sections -ffunction-sections

LIBS += -lc -lm 
LIBDIR = 
LDFLAGS = $(MCU) -T$(LDSCRIPT) $(LIBDIR) $(LIBS) -Wl,-Map=$(BUILD_DIR)/$(TARGET).map,--cref -Wl,--gc-sections

## Objects
# Get objects from C file names (without directory information, flatten out)
OBJECTS = $(addprefix $(BUILD_DIR)/$(OBJDIR)/,$(notdir $(C_SOURCES:.c=.o)))
DEP_FILES := $(OBJECTS:.o=.d)
# Search for C sources as path information is no longer available
vpath %.c $(sort $(dir $(C_SOURCES)))
# Get objects from S file names (without directory information, flatten out)		
OBJECTS += $(addprefix $(BUILD_DIR)/$(OBJDIR)/,$(notdir $(ASM_SOURCES:.s=.o)))
# Search for ASM sources as path information is no longer available
vpath %.s $(sort $(dir $(ASM_SOURCES)))


## Stack usage files
SU_FILES = $(OBJECTS:.o=.su)

## Build

# Outputs
VER = $(SRC)/version.h 
ELF = $(BUILD_DIR)/$(TARGET).elf
HEX = $(BUILD_DIR)/$(TARGET).hex
BIN = $(BUILD_DIR)/$(TARGET).bin
MAP = $(BUILD_DIR)/$(TARGET).map
EXE = $(BUILD_DIR)/$(TARGET).exe

# Dependencies	
-include $(DEP_FILES)

all: $(pall)

# Generating version file
$(VER):
	@echo Generating $@
	@echo /* THIS IS A GENERATED FILE, DO NOT EDIT IT! */  > $@
	@echo #define VERSION_SHORT 	"$(VERSION_SHORT)" >> $@
	@echo #define VERSION 		"$(VERSION)-$(BRANCH)-$(COMMITDATETIME)" >> $@

$(BUILD_DIR)/$(OBJDIR)/%.o: %.c 
	$(CC) $< -o $@ $(CFLAGS) 
	
$(BUILD_DIR)/$(OBJDIR)/%.o: %.s
	$(AS) $< -o $@ $(ASFLAGS) 

$(ELF): $(OBJECTS)
	$(LD) $(OBJECTS) $(LDFLAGS) $(LDLIBS) -o $@ 
	$(SZ) $@

$(EXE): $(OBJECTS)
	$(LD) $(OBJECTS) $(LDLIBS) -o $@ 
	$(SZ) $@
	
$(HEX): $(ELF)
	$(OC) -O ihex $< $@
	
$(BIN): $(ELF)
	$(OC) -O binary -S $< $@

path:
#	mkdir -p $(BUILD_DIR)
#	mkdir -p $(BUILD_DIR)/obj
	@-md $(subst /,\,$(BUILD_DIR)) > nul 2>&1 ||: 
	@-md $(subst /,\,$(BUILD_DIR)/obj) > nul 2>&1 ||:

.PHONY: clean
clean:
	rm -rf $(OBJECTS) $(DEP_FILES) $(SU_FILES) $(ELF) $(HEX) $(BIN) $(EXE) $(MAP) $(VER)
	
	