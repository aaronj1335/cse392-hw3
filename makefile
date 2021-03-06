CC = g++
GCC = gcc
DB = lldb
FLAGS = -Wall -Werror
LIBRARIES = -fopenmp -ltbb
BIN_DIR = bin
Q2_TARGET_BASE = nbody
TEST_TARGET_BASE = test
Q2_TARGET = $(BIN_DIR)/$(Q2_TARGET_BASE)
TEST_TARGET = $(BIN_DIR)/$(TEST_TARGET_BASE)

ifeq ($(shell uname), Darwin)
	CC = g++-4.8
endif

ifeq ($(DEBUG), 1)
	FLAGS += -g
endif

# on TACC the only thing i've figured out is to download the intel tbb archive
# from here:
#
# 	https://threadingbuildingblocks.org/sites/default/files/software_releases/linux/tbb42_20140122oss_lin.tgz
#
# and then unzip it into ~/download/, then the makefile will link it from the
# shared library there using the conditional below
ifneq ($(wildcard $(HOME)/download/tbb42_20140122oss),)
	LIBRARIES := -I$(HOME)/download/tbb42_20140122oss/include -L$(HOME)/download/tbb42_20140122oss/lib/intel64/gcc4.4 $(LIBRARIES)
	LD_LIBRARY_PATH:=$(LD_LIBRARY_PATH):/home1/01649/astacy/download/tbb42_20140122oss/lib/intel64/gcc4.4
endif

SRC_DIR = src
OBJ_DIR = obj
SCRATCH ?= .
VAR_DIR = $(SCRATCH)/var
RESULTS_DIR = results

INPUTS = $(wildcard $(SRC_DIR)/*.cpp)
INPUTS_TMP = $(subst $(SRC_DIR),$(OBJ_DIR),$(INPUTS))
OBJECTS = $(INPUTS_TMP:%.cpp=%.o)
Q2_OBJECTS = $(filter-out $(OBJ_DIR)/$(TEST_TARGET_BASE).o, $(OBJECTS))
TEST_OBJECTS = $(filter-out $(OBJ_DIR)/$(Q2_TARGET_BASE).o, $(OBJECTS))
DEPFILES = $(OBJECTS:%.o=%.d)

# ifeq ($(OMP_NUM_THREADS), 1)
# 	PERF_SUBDIR = core
# endif

# ifeq ($(OMP_NUM_THREADS), 6)
# 	PERF_SUBDIR = socket
# endif

# ifeq ($(OMP_NUM_THREADS), 12)
# 	PERF_SUBDIR = node
# endif

REPORT_HTML = report/report.html
REPORT_SRC = report/report.md

# main application

all: $(Q2_TARGET) $(TEST_TARGET)

$(Q2_TARGET): $(Q2_OBJECTS) | $(BIN_DIR)
	$(CC) $(FLAGS) $(LIBRARIES) -o $@ $(Q2_OBJECTS)

$(TEST_TARGET): $(TEST_OBJECTS) | $(BIN_DIR)
	$(CC) $(FLAGS) $(LIBRARIES) -o $@ $(TEST_OBJECTS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CC) $(FLAGS) $(LIBRARIES) -c -o $@ $<

$(OBJ_DIR)/%.d: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CC) $(FLAGS) $(LIBRARIES) -M -MF $@ -MT $(@:%.d=%.o) $<

$(OBJ_DIR):
	mkdir $(OBJ_DIR)

$(BIN_DIR):
	mkdir $(BIN_DIR)


# running, testing, etc

test: all
	@export OMP_NUM_THREADS=2 && ./$(TEST_TARGET)
	@export OMP_NUM_THREADS=4 && ./$(TEST_TARGET)
	@./$(Q2_TARGET)

run: all
	@export OMP_NUM_THREADS=4 && ./$(Q2_TARGET)

run-small: all
	@export OMP_NUM_THREADS=4 && ./$(Q2_TARGET) -s

$(RESULTS_DIR):
	mkdir $@

perf: all | $(RESULTS_DIR)
	@export RESULTS_DIR=$(RESULTS_DIR) && bin/bench.sh


# meeeeeeeetttttttaaaaaa

tags: $(INPUTS)
	ctags -R $(SRC_DIR)

# note that when doing this, there's a good chance you'll need to
# `make clean && DEBUG=1 make debug`, and then `DEBUG=1 make debug` on all
# subsequent commands
debug: all
	@export OMP_NUM_THREADS=4 && lldb -- ./$(Q2_TARGET) -s

# report

report: $(REPORT_HTML)

$(REPORT_HTML): $(REPORT_SRC) etc/template.html etc/marked.js
	python etc/buildreport.py < $< > $@


# submission -- abhi you'll need to update this accordingly

zip: clean
	rm -rf ../cse392asab_stacy || true
	mkdir ../cse392asab_stacy
	cp -r ./ ../cse392asab_stacy/
	rm -rf ../cse392asab_stacy/.git
	(cd ../ && zip -r cse392asab_stacy_aaron_hwk2.zip cse392asab_stacy )
	rm -rf ../cse392asab_stacy

zip_abhi: clean
	rm -rf ../cse392asab_bhaduri || true
	mkdir ../cse392asab_bhaduri
	cp -r ./ ../cse392asab_bhaduri/
	rm -rf ../cse392asab_bhaduri/.git
	(cd ../ && zip -r cse392asab_bhaduri_abhishek_hwk2.zip cse392asab_bhaduri )
	rm -rf ../cse392asab_bhaduri

# cleanup

.PHONY: clean
clean:
	-rm -rf $(OBJ_DIR) $(VAR_DIR) $(Q2_TARGET) $(TEST_TARGET)

-include $(DEPFILES)

