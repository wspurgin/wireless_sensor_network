###################################################################
#  Makefile for PCA of sdssD7
#
#  Will Spurgin
#  SMU Mathematics & Computer Science
#  Math 4370/6370
#  21 April 2015
###################################################################

# compiler options  & flags
UNAME := $(shell uname)

ifeq ($(UNAME), Darwin)
        CXX = g++-5
else
        CXX = g++
endif

CXXFLAGS = -fopenmp -O2 -std=c++11

# All lib objects
ALL = mat.o vec.o

# Get current working directory
# and specify lib directory

MKFILE_PATH := $(abspath $(lastword $(MAKEFILE_LIST)))
MAKE_DIR := $(dir $(MKFILE_PATH))
BIN_DIR := $(MAKE_DIR)bin/
LIB_DIR = $(MAKE_DIR)lib/

ALL := $(addprefix $(BIN_DIR), $(ALL))

# makefile targets
all : wsn

$(BIN_DIR)gen_rgg.out : $(MAKE_DIR)gen_rgg.cpp $(ALL) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $(MAKE_DIR)gen_rgg.cpp $(ALL) -o $@

$(BIN_DIR)mat.o : $(LIB_DIR)mat.cpp | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -c $(LIB_DIR)mat.cpp -o $@

$(BIN_DIR)vec.o : $(LIB_DIR)vec.cpp | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -c $(LIB_DIR)vec.cpp -o $@

$(BIN_DIR):
	@echo "making directory $(BIN_DIR)"
	mkdir $(BIN_DIR)

clean:
	rm -rf $(BIN_DIR)

.PHONEY: print_vars, search_engine, copy_ui_man
print_vars:
	@echo "CXX => $(CXX)"
	@echo "CXXFLAGS => $(CXXFLAGS)"
	@echo "MAKE_DIR => $(MAKE_DIR)"
	@echo "LIB_DIR => $(LIB_DIR)"
	@echo "BIN_DIR => $(BIN_DIR)"
	@echo "ALL => $(ALL)"

wsn: $(BIN_DIR)gen_rgg.out
	@echo "Finished building Wireless Sensory Network"
####### End of Makefile #######
