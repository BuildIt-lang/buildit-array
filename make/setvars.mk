#if debug is not set, default to false
DEBUG?=0

# Query CFLAGS and LD_FLAGS from BuildIt
# BuildIt doesn't have to be built for this

ifeq ($(DEBUG), 1)
CFLAGS=-O0 -g
else 
CFLAGS=-O3
endif
LDFLAGS=

LDFLAGS+=-L$(BUILD_DIR) -l$(LIBRARY_NAME)

CFLAGS+=$(shell make --no-print-directory -C $(BUILDIT_DIR)/ DEBUG=$(DEBUG) compile-flags)
LDFLAGS+=$(shell make --no-print-directory -C $(BUILDIT_DIR)/ DEBUG=$(DEBUG) linker-flags)

DEPS=$(BUILD_DIR)/buildit.dep
INCLUDE_FLAGS=-I $(INCLUDE_DIR)

LIBRARY=$(BUILD_DIR)/lib$(LIBRARY_NAME).a

