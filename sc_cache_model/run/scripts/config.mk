
## Variable that points to SystemC installation path
SYSTEMC_HOME ?= /open_tools/accellera/systemc/3.0.1

## Select the target architecture
TARGET_ARCH = linux64

## Select the architecture suffix, if necessary
ARCH_SUFFIX = -$(TARGET_ARCH)

## How to instruct the dynamic linker to locate the SystemC library
LDFLAG_RPATH = -Wl,-rpath=

# default compiler flags
FLAGS_COMMON += -g
FLAGS_COMMON += -Wall
FLAGS_STRICT += -Wextra
# FLAGS_STRICT += -pedantic
FLAGS_STRICT += -Wno-long-long
FLAGS_WERROR += -Werror

# combined flags
SYSTEMC_CXXFLAGS = $(FLAGS_COMMON) $(FLAGS_STRICT) $(FLAGS_WERROR)

# Additional preprocessor symbols
SYSTEMC_DEFINES =

# Explicit location of the SystemC headers
SYSTEMC_INC_DIR = $(SYSTEMC_HOME)/include

# Explicit location if the SystenC library
SYSTEMC_LIB_DIR = $(SYSTEMC_HOME)/lib$(ARCH_SUFFIX)

# Run with valgrind
VALGRIND=valgrind --log-file-exactly=valgrind.log

