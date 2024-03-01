-include Makefile.inc

LIBRARY_NAME=barray

include make/dirs.mk
include make/setvars.mk
include make/verbose.mk

all: executables $(LIBRARY)

CHECK_CONFIG=1
CONFIG_STR=DEBUG=$(DEBUG) ENABLE_D2X=$(ENABLE_D2X)

include make/stable_config.mk
include make/deps.mk
include make/rules.mk

.PHONY: test
test: $(BUILD_DIR)/sample1
	$(BUILD_DIR)/sample1 > $(BUILD_DIR)/test.cpp
	g++ $(BUILD_DIR)/test.cpp test/driver.cpp -I $(BUILDIT_DIR)/deps/d2x/include $(BUILDIT_DIR)/deps/d2x/runtime/d2x_runtime.cpp $(BUILDIT_DIR)/deps/d2x/src/utils.cpp -fpermissive -ldwarf -ldl -lunwind -o $(BUILD_DIR)/test -g -O0 -gdwarf-4

clean:
	- rm -rf $(BUILD_DIR)
