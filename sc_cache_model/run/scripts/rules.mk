ifndef SYSTEMC_LIB_DIR
$(error "Makefile variable SYSTEMC_LIB_DIR is not provided, can't continue, check config.mk")
endif
ifndef LDFLAG_RPATH
$(error "Makefile variable LDFLAG_RPATH is not provided, can't continue, check config.mk")
endif

## ***************************************************************************

SYSTEMC_LDFLAGS  += $(foreach DIR, $(SYSTEMC_LIB_DIR), $(addprefix -L , $(DIR)))
SYSTEMC_LDFLAGS  += $(LDFLAG_RPATH)$(SYSTEMC_LIB_DIR)
SYSTEMC_LIBS     ?= -lsystemc -lm

## Add 'PTHREADS=1' to command line for a pthreads build
## (should not be needed in most cases)
ifdef PTHREADS
SYSTEMC_CXXFLAGS += -pthread
SYSTEMC_LIBS     += -lpthread
endif

ifndef SYSTEMC_INC_DIR
$(error "Makefile variable SYSTEMC_INC_DIR is not provided, can't continue, check config.mk")
endif
INCDIR   += $(SYSTEMC_INC_DIR)
LIBDIR   += 

CXXFLAGS  += $(CFLAGS) $(SYSTEMC_CXXFLAGS) $(SYSTEMC_DEFINES)
CXXFLAGS  += $(foreach DIR, $(INCDIR), $(addprefix -I, $(DIR)))
CXXFLAGS  += $(foreach DEFINE, $(SYSTEMC_DEFINES), $(addprefix -D, $(DEFINE)))

LDFLAGS   += $(CFLAGS) $(SYSTEMC_CXXFLAGS) $(SYSTEMC_LDFLAGS)
LDFLAGS   += $(foreach DIR, $(LIBDIR), $(addprefix -L, $(DIR)))
LIBS      += $(SYSTEMC_LIBS) $(EXTRA_LIBS)

TEST_NAMES = untagged_msg tagged_msg tagged_msg_any_source unexpected_msg unexpected_tagged_msg deferred_msg deferred_tagged_msg deferred_tagged_any_source

ifndef PROJECT
$(error "Makefile variable PROJECT is not provided, can't continue, check Makefile")
endif


## ***************************************************************************
## build rules

EXEEXT?=.x
EXE   := $(PROJECT)$(EXEEXT)

all: build


run: all
	eval "./$(EXE) $(ARGS)"


build: $(EXE)


$(EXE): $(OBJS)
	$(CXX) \
		$(LDFLAGS) \
		-o $@ \
		$(OBJS) \
		$(LIBS) \
		2>&1 | c++filt
	test -x $@


$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(@D)
	$(CXX) \
		$(CXXFLAGS) \
		-c $< \
		-o $@

run_tests:
	@for test in $(TEST_NAMES); do \
		echo "Running test $$test"; \
		make run TEST_ROLE=$(TEST_ROLE) TEST_NAME=$$test PEER_IP=$(PEER_IP); \
	done


clean::
	rm -f $(OBJS) $(EXE) core $(EXTRA_CLEAN) \
		run.log run_trimmed.log expected_trimmed.log diff.log
