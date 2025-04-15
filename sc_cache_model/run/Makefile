ORIGIN_DIR  ?= .

CXX := g++
CC  := gcc

MAKEFILE_FILE   := $(lastword $(MAKEFILE_LIST))
MAKEFILE_HOME   := $(shell realpath --relative-to $(ORIGIN_DIR) $(dir $(MAKEFILE_FILE)))

PROJECT_TOP_DIR := $(MAKEFILE_HOME)/..

SRC_DIR         := $(shell realpath --relative-to $(ORIGIN_DIR) $(PROJECT_TOP_DIR)/src)
BUILD_DIR       := $(shell realpath --relative-to $(ORIGIN_DIR) $(PROJECT_TOP_DIR)/run/build)


include $(MAKEFILE_HOME)/scripts/config.mk

PROJECT    := cache_model

INCDIR += $(SRC_DIR)/sc_cache_model
INCDIR += $(SRC_DIR)/sc_cache_tests/interface/
INCDIR += $(SRC_DIR)/sc_cache_tests
INCDIR += $(SRC_DIR)/utility
INCDIR += $(SRC_DIR)

SRCS    := $(wildcard $(SRC_DIR)/sc_cache_model/*.cpp)
SRCS    += $(wildcard $(SRC_DIR)/sc_cache_tests/interface/*.cpp)

SRCS    += $(wildcard $(SRC_DIR)/sc_cache_tests/*.cpp)
SRCS    += $(wildcard $(SRC_DIR)/utility/*.cpp)
SRCS    += $(wildcard  $(SRC_DIR)/testbench.cpp)
SRCS    += $(wildcard $(SRC_DIR)/source.cpp)

OBJS    := $(subst $(SRC_DIR)/,$(BUILD_DIR)/,$(SRCS:.cpp=.o))


$(info SRC_DIR   = $(SRC_DIR))
$(info SRC       = $(SRCS))
$(info BUILD_DIR = $(BUILD_DIR))
$(info OBJS      = $(OBJS))

CFLAGS += -g
CFLAGS += -Wno-unused-parameter
CFLAGS += -Wno-unused-but-set-variable
CFLAGS += -Wno-deprecated-declarations
CFLAGS += -Wno-c++20-extensions
CFLAGS += -Wno-missing-field-initializers
CFLAGS += -Wno-reorder
CFLAGS += -Wno-overloaded-virtual
CFLAGS += -Wno-sign-compare
CFLAGS += -Wno-error=overloaded-virtual
CFLAGS += -Wno-error=sign-compare



VERBOSITY_LEVEL    		?= "SC_MEDIUM"
TEST_NAME          		?= "cache_bulk_read_write_check"
CACHE_SIZE         		?= "32768"
LINE_SIZE         		?= "64"
SET_ASSOC_COUNT    		?= "4"
WORD_SIZE          		?= "4"
MEMORY_READ_DELAY  		?= "40"
MEMORY_WRITE_DELAY 		?= "60"
CACHE_READ_DELAY   		?= "4"
CACHE_WRITE_DELAY  		?= "6"
CACHE_INVALIDATE_DELAY  ?= "3"
CONNECT_TO_EXTERNAL_MEMORY ?= "false"


override ARGS += --verbosity_level=$(VERBOSITY_LEVEL)
override ARGS += --test_name=$(TEST_NAME)
override ARGS += --cache_size=$(CACHE_SIZE)
override ARGS += --line_size=$(LINE_SIZE)
override ARGS += --set_assoc_count=$(SET_ASSOC_COUNT)
override ARGS += --word_size=$(WORD_SIZE)
override ARGS += --memory_read_delay=$(MEMORY_READ_DELAY)
override ARGS += --memory_write_delay=$(MEMORY_WRITE_DELAY)
override ARGS += --cache_read_delay=$(CACHE_READ_DELAY)
override ARGS += --cache_write_delay=$(CACHE_WRITE_DELAY)
override ARGS += --cache_invalidate_delay=$(CACHE_INVALIDATE_DELAY)
override ARGS += --connect_to_external_memory=$(CONNECT_TO_EXTERNAL_MEMORY)


include $(MAKEFILE_HOME)/scripts/rules.mk

clean::
	rm -rf $(BUILD_DIR)
