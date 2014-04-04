CC = g++
GCC = gcc
FLAGS = -Wall -Werror
LIBRARIES = -fopenmp
BIN_DIR = bin
Q2_TARGET_BASE = nbody
TEST_TARGET_BASE = test
Q2_TARGET = $(BIN_DIR)/$(Q2_TARGET_BASE)
TEST_TARGET = $(BIN_DIR)/$(TEST_TARGET_BASE)

ifeq ($(shell uname), Darwin)
	CC = g++-4.8
endif

SRC_DIR = src
OBJ_DIR = obj
SCRATCH ?= .
VAR_DIR = $(SCRATCH)/var

INPUTS = $(wildcard $(SRC_DIR)/*.cc)
INPUTS_TMP = $(subst $(SRC_DIR),$(OBJ_DIR),$(INPUTS))
OBJECTS = $(INPUTS_TMP:%.cc=%.o)
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

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cc | $(OBJ_DIR)
	$(CC) $(FLAGS) $(LIBRARIES) -c -o $@ $<

$(OBJ_DIR)/%.d: $(SRC_DIR)/%.cc | $(OBJ_DIR)
	$(CC) $(FLAGS) $(LIBRARIES) -M -MF $@ -MT $(@:%.d=%.o) $<

$(OBJ_DIR):
	mkdir $(OBJ_DIR)

$(BIN_DIR):
	mkdir $(BIN_DIR)


# running, testing, etc

test: all
	@./$(TEST_TARGET)

$(VAR_DIR):
	mkdir $@

$(VAR_DIR)/1d.txt: $(VAR_DIR)
	@python test/perfdata.py 1 > $@

$(VAR_DIR)/4d.txt: $(VAR_DIR)
	@python test/perfdata.py 4 > $@

# this is not used b/c it was slow and when doing perf we could care less about
# the actual values
perfdata: $(PERF_FILES)

$(RESULTS_DIR):
	mkdir -p $@

perf: all | $(RESULTS_DIR)
	time ./$(Q2_TARGET) -nm 1        1>/dev/null 3>$(RESULTS_DIR)/1d_0001M.txt
	time ./$(Q2_TARGET) -nm 10       1>/dev/null 3>$(RESULTS_DIR)/1d_0010M.txt
	time ./$(Q2_TARGET) -nm 100      1>/dev/null 3>$(RESULTS_DIR)/1d_0100M.txt
	time ./$(Q2_TARGET) -nm 300      1>/dev/null 3>$(RESULTS_DIR)/1d_0300M.txt
	time ./$(Q2_TARGET) -nm 1000     1>/dev/null 3>$(RESULTS_DIR)/1d_1000M.txt
	time ./$(Q2_TARGET) -nm 300 -d 4 1>/dev/null 3>$(RESULTS_DIR)/4d_0300M.txt

scaling: all | $(RESULTS_DIR)
	time ./$(Q2_TARGET) -nm 10       1>/dev/null 3>$(RESULTS_DIR)/1d_0010M.txt
	time ./$(Q2_TARGET) -nm 60       1>/dev/null 3>$(RESULTS_DIR)/1d_0060M.txt
	time ./$(Q2_TARGET) -nm 120      1>/dev/null 3>$(RESULTS_DIR)/1d_0120M.txt


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

