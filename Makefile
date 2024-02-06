-include Makefile.inc

LIBRARY_NAME=foo

include make/dirs.mk
include make/setvars.mk
include make/verbose.mk

all: executables $(LIBRARY)

CHECK_CONFIG=1
CONFIG_STR=DEBUG=$(DEBUG)

include make/stable_config.mk
include make/deps.mk
include make/rules.mk

clean:
	- rm -rf $(BUILD_DIR)
