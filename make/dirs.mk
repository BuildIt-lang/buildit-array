PWD=$(shell pwd)

BUILD_DIR?=$(PWD)/build
INCLUDE_DIR=$(PWD)/include
SRC_DIR=$(PWD)/src

BUILDIT_DIR?=$(PWD)/deps/buildit
BUILDIT_DEPS=$(wildcard $(BUILDIT_DIR)/src/*) $(wildcard $(BUILDIT_DIR)/include/*) 

SAMPLES_DIR=$(PWD)/samples
SAMPLES_SRCS=$(wildcard $(SAMPLES_DIR)/*.cpp)
SAMPLES=$(subst $(SAMPLES_DIR),$(BUILD_DIR),$(SAMPLES_SRCS:.cpp=))

LIBRARY_SRCS=$(wildcard $(SRC_DIR)/*.cpp)
LIBRARY_OBJS=$(subst $(SRC_DIR),$(BUILD_DIR),$(LIBRARY_SRCS:.cpp=.o))

INCLUDES=$(wildcard $(INCLUDE_DIR)/*.h)

