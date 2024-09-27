# ----------------------------------------
# Copyright (C) 2024 Grovf Inc. All Rights Reserved.
#
# This is the project-specific Makefile for modelling 
# of Grovf Home-grown cache_model project
#
# Contributors:
# D.Margaryan david.m@grovf.com
# N.Zibilyuk nazar.z@grovf.com
# ----------------------------------------
#
#  TODO:
#  1) gather debug database for visualiser
#  1) make compatible with gcc compilation (lurk for 'FIXME', 'gcc')

.DEFAULT_GOAL := all

TRACE_OPT := 0

LOG_DIR ?= results

MAKEFILE_FILE    := $(lastword $(MAKEFILE_LIST))
MAKEFILE_HOME    := $(dir $(MAKEFILE_FILE))
ROOT_DIR  		 := $(MAKEFILE_HOME)/..


SC_DIR := $(ROOT_DIR)/src

ifndef UVMC_HOME
$(error "Environment variable UVMC_HOME is not set")
endif
UVMC_LIB ?= $(UVMC_HOME)/lib/uvmc_lib


VERBOSITY_LEVEL    		?= "SC_MEDIUM"
TEST_NAME          		?= "write_read_check"
CACHE_SIZE         		?= "32768"
BLOCK_SIZE         		?= "64"
SET_ASSOC_COUNT    		?= "4"
WORD_SIZE          		?= "4"
WRITE_POLICY       		?= "WRITE_BACK"
MEMORY_READ_DELAY  		?= "40"
MEMORY_WRITE_DELAY 		?= "60"
CACHE_READ_DELAY   		?= "4"
CACHE_WRITE_DELAY  		?= "6"
CACHE_INVALIDATE_DELAY  ?= "3"
CONNECT_TO_EXTERNAL_MEMORY ?= "false"

# List of command line arguments
SYSTEMC_CMDLINE_VARS := VERBOSITY_LEVEL    		\
    					TEST_NAME 		   		\
    					CACHE_SIZE 		   		\
    					BLOCK_SIZE 		   		\
    					SET_ASSOC_COUNT    		\
    					WORD_SIZE 		   		\
    					WRITE_POLICY       		\
						MEMORY_READ_DELAY  		\
						MEMORY_WRITE_DELAY 		\
						CACHE_READ_DELAY   		\
						CACHE_WRITE_DELAY  		\
						CACHE_INVALIDATE_DELAY  \
						CONNECT_TO_EXTERNAL_MEMORY


SYSTEMC_CMDLINE_ARGS := $(foreach var,$(SYSTEMC_CMDLINE_VARS),-sc_arg $($(var)))

# CAUTION: This always shall use the same addressing (absolute or relative) what
#          is used for UVMC_HOME, i.e. it is better to never translate it to relative
INCDIRS += $(UVMC_HOME)/src/connect/sc

# This could be relative or absolute depending on other variables
INCDIRS += $(SC_DIR)/sc_cache_model
INCDIRS += $(SC_DIR)/sc_cache_tests/interface/
INCDIRS += $(SC_DIR)/sc_cache_tests
INCDIRS += $(SC_DIR)/utility
INCDIRS += $(SC_DIR)

# CAUTION: build is made for sccom only so far
#          where is no way to build object filenames with globs such as * with mere
#          test substitution, and makefile can only do this.
# TODO: use 'find' to build full source list for gcc compilation
SC_MODEL_SRC += $(SC_DIR)/sc_cache_model/*.cpp
SC_MODEL_SRC += $(SC_DIR)/sc_cache_tests/interface/*.cpp

# TODO: use 'find' to build full source list for gcc compilation
SC_ENV_SRC += $(SC_DIR)/sc_cache_tests/*.cpp
SC_ENV_SRC += $(SC_DIR)/utility/*.cpp
SC_ENV_SRC += $(SC_DIR)/testbench.cpp
SC_ENV_SRC += $(SC_DIR)/source.cpp

########################################################################################

SC_COMPILE_OPTS += -64

SC_COMPILE_OPTS += $(foreach INCDIR, $(INCDIRS), $(addprefix -I, $(INCDIR)))

SC_COMPILE_OPTS += -DSC_INCLUDE_DYNAMIC_PROCESSES
SC_COMPILE_OPTS += -DQUESTA

SC_LINK_OPTS += -64
SC_LINK_OPTS += -link
SC_LINK_OPTS += -lib $(UVMC_LIB)
SC_LINK_OPTS += -lib work


########################################################################################

SIM_OPTS += -64
SIM_OPTS += -batch
SIM_OPTS += -do "run -all; quit -f"
SIM_OPTS += -t 1ps
SIM_OPTS += -L $(UVMC_LIB)



ifeq ( $(TRACE), 1 )
	SIM_OPTS += +UVMC_COMMAND_TRACE
endif


.PHONY: all
all: sc_compile_model sc_compile_env sc_link sim


.PHONY: sc_compile_model
sc_compile_model: $(LOG_DIR)
	sccom                     	\
		$(SC_COMPILE_OPTS)    	\
		$(SC_MODEL_SRC)         \
		-log $(LOG_DIR)/$@.log


.PHONY: sc_compile_env
sc_compile_env: $(LOG_DIR)
	sccom                     	\
		$(SC_COMPILE_OPTS)    	\
		$(SC_ENV_SRC)           \
		-log $(LOG_DIR)/$@.log


.PHONY: sc_link
sc_link: $(LOG_DIR)
	sccom                     \
		$(SC_LINK_OPTS)       \
		-log $(LOG_DIR)/$@.log


.PHONY: sim
sim: $(LOG_DIR)
	vsim                        \
		$(SYSTEMC_CMDLINE_ARGS) \
 		$(SIM_OPTS)             \
		sc_main		            \
		-l $(LOG_DIR)/$@.log


$(LOG_DIR):
	@mkdir -pv $@


#TODO: implement `check_test_passing` recipe


.PHONY: clean
clean:
	@rm -rf results/
	@rm -rf work/
	@rm -rf tr_db.log
	@rm -rf *.vstf


